#include <algorithm>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

struct WeatherData
{
  double temperatureC = 0.0;
  double humidityPct = 0.0;
  double pressureHPa = 0.0;
};

class IObserver
{
  public:
    virtual ~IObserver() = default;
    virtual void update(const WeatherData& data) = 0;
    virtual std::string name() const = 0;
};

class WeatherStation;

class Subscription
{
  public:
    Subscription() = default;
    Subscription(std::weak_ptr<WeatherStation> station, std::size_t id)
        : station_(std::move(station)), id_(id), active_(true) {}

    ~Subscription() { reset(); }

    Subscription(const Subscription&) = delete;
    Subscription& operator=(const Subscription&) = delete;

    Subscription(Subscription&& other) noexcept { moveFrom(other); }
    Subscription& operator=(Subscription&& other) noexcept
    {
      if (this != &other)
      {
        reset();
        moveFrom(other);
      }
      return *this;
    }

    void reset()
    {
      if (!active_) { return; }
      if (auto station = station_.lock()) { detachFrom(*station); }
      active_ = false;
      id_ = 0;
      station_.reset();
    }

    bool active() const { return active_; }

  private:
    void detachFrom(WeatherStation& station);

    void moveFrom(Subscription& other)
    {
      station_ = std::move(other.station_);
      id_ = other.id_;
      active_ = other.active_;
      other.active_ = false;
      other.id_ = 0;
    }

    std::weak_ptr<WeatherStation> station_;
    std::size_t id_ = 0;
    bool active_ = false;
};

class WeatherStation : public std::enable_shared_from_this<WeatherStation>
{
  public:
    Subscription attach(std::weak_ptr<IObserver> observer)
    {
      const std::size_t id = nextId_++;
      observers_.push_back(Entry{id, std::move(observer)});
      return Subscription(weak_from_this(), id);
    }

    void detach(std::size_t id)
    {
      observers_.erase(std::remove_if(observers_.begin(), observers_.end(),
        [id](const Entry& e) { return e.id == id; }), observers_.end());
    }

    void setMeasurements(double temperatureC, double humidityPct, double pressureHPa)
    {
      data_ = {temperatureC, humidityPct, pressureHPa};
      notify();
    }

    const WeatherData& data() const { return data_; }

    void notify()
    {
      auto it = observers_.begin();
      while (it != observers_.end())
      {
        if (auto obs = it->observer.lock())
        {
          obs->update(data_);
          ++it;
        }
        else
        {
          std::cout << "[Station] Removing expired observer subscription #" << it->id
            << "\n";
          it = observers_.erase(it);
        }
      }
    }

    std::size_t observerCount() const { return observers_.size(); }

  private:
    struct Entry
    {
      std::size_t id = 0;
      std::weak_ptr<IObserver> observer;
    };

    WeatherData data_{};
    std::vector<Entry> observers_;
    std::size_t nextId_ = 1;
};

void Subscription::detachFrom(WeatherStation& station) { station.detach(id_); }

class ConsoleDisplay : public IObserver
{
  public:
    explicit ConsoleDisplay(std::string label) : label_(std::move(label)) {}
    void update(const WeatherData& data) override
    {
      std::cout << "[" << label_ << "] T=" << std::fixed << std::setprecision(1)
        << data.temperatureC << "C, H=" << data.humidityPct
        << "%, P=" << data.pressureHPa << "hPa\n";
    }
    std::string name() const override { return label_; }

  private:
    std::string label_;
};

class StatisticsDisplay : public IObserver
{
  public:
    void update(const WeatherData& data) override
    {
      ++count_;
      sumTemp_ += data.temperatureC;
      if (count_ == 1 || data.temperatureC < minTemp_) minTemp_ = data.temperatureC;
      if (count_ == 1 || data.temperatureC > maxTemp_) maxTemp_ = data.temperatureC;
      std::cout << "[StatisticsDisplay] samples=" << count_ << ", avgT=" << std::fixed
        << std::setprecision(2) << (sumTemp_ / static_cast<double>(count_))
        << "C, minT=" << minTemp_ << "C, maxT=" << maxTemp_ << "C\n";
    }
    std::string name() const override { return "StatisticsDisplay"; }

  private:
    int count_ = 0;
    double sumTemp_ = 0.0;
    double minTemp_ = 0.0;
    double maxTemp_ = 0.0;
};

class AlertDisplay : public IObserver
{
  public:
    void update(const WeatherData& data) override
    {
      std::cout << "[AlertDisplay] ";
      bool alert = false;
      if (data.temperatureC > 35.0)
      {
        std::cout << "HEAT ALERT! ";
        alert = true;
      }
      if (data.humidityPct > 80.0)
      {
        std::cout << "HIGH HUMIDITY! ";
        alert = true;
      }
      if (data.pressureHPa < 980.0)
      {
        std::cout << "LOW PRESSURE! ";
        alert = true;
      }
      if (!alert)
      {
        std::cout << "All values within normal range.";
      }
      std::cout << '\n';
    }
    std::string name() const override { return "AlertDisplay"; }

  private:
};

int main()
{
  std::cout << "__ Task 2: Weather Station (Observer) __\n\n";

  auto station = std::make_shared<WeatherStation>();

  auto console = std::make_shared<ConsoleDisplay>("ConsoleDisplay");
  auto stats = std::make_shared<StatisticsDisplay>();
  auto alert = std::make_shared<AlertDisplay>();

  Subscription subConsole = station->attach(console);
  Subscription subStats = station->attach(stats);
  Subscription subAlert = station->attach(alert);

  std::cout << "Attached 3 observers.\n\n";
  station->setMeasurements(22.5, 55.0, 1013.0);

  std::cout << "\n__ Update 2: hot and humid __\n";
  station->setMeasurements(36.2, 85.0, 1005.0);

  std::cout << "\n__ Detach statistics via RAII token.reset() __\n";
  subStats.reset();
  station->setMeasurements(28.0, 60.0, 990.0);

  std::cout << "\n__ Delete alert observer object (no manual detach) __\n";
  alert.reset();
  std::cout << "Alert shared_ptr use_count is 0; station should not crash.\n";
  station->setMeasurements(18.0, 70.0, 975.0);

  std::cout << "\n__ RAII token auto-detach demo __\n";
  {
    auto tempObserver = std::make_shared<ConsoleDisplay>("TemporaryDisplay");
    Subscription tempSub = station->attach(tempObserver);
    station->setMeasurements(20.0, 50.0, 1010.0);
    std::cout << "Leaving scope: tempSub destructor will detach automatically.\n";
  }
  station->setMeasurements(21.0, 51.0, 1011.0);

  std::cout << "\n__ Detach remaining console token explicitly __\n";
  subConsole.reset();
  station->setMeasurements(25.0, 55.0, 1012.0);
  std::cout << "No observers notified; observerCount=" << station->observerCount() << "\n";

  std::cout << "\nDemo complete: no dangling pointers, no crashes on deleted observers.\n";
  return 0;
}
