#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

struct GameEvent
{
    std::string type;
    std::string source;
    std::string details;
};

class IGameEventObserver
{
  public:
    virtual ~IGameEventObserver() = default;
    virtual void onEvent(const GameEvent& event) = 0;
    virtual std::string name() const = 0;
};

class EventBus
{
  public:
    void subscribe(std::shared_ptr<IGameEventObserver> observer)
    {
      observers_.push_back(std::move(observer));
    }

    void publish(const GameEvent& event) const
    {
      std::cout << "[EventBus] " << event.type << " from " << event.source << ": "
        << event.details << '\n';
      for (const auto& w : observers_)
      {
        if (auto obs = w.lock()) { obs->onEvent(event); }
      }
    }

  private:
    std::vector<std::weak_ptr<IGameEventObserver>> observers_;
};

class LoggingObserver : public IGameEventObserver
{
  public:
    void onEvent(const GameEvent& event) override
    {
      log_.push_back(event.type + ":" + event.source);
      std::cout << "  [LogObserver] recorded " << event.type << "\n";
    }
    std::string name() const override { return "LoggingObserver"; }
    const std::vector<std::string>& log() const { return log_; }

  private:
    std::vector<std::string> log_;
};

class AchievementObserver : public IGameEventObserver
{
  public:
    void onEvent(const GameEvent& event) override
    {
      if (event.type == "death")
      {
        ++deaths_;
        std::cout << "  [AchievementObserver] deaths observed: " << deaths_ << "\n";
      }
      if (event.type == "collision")
      {
        ++collisions_;
        std::cout << "  [AchievementObserver] collisions observed: " << collisions_ << "\n";
      }
    }
    std::string name() const override { return "AchievementObserver"; }

  private:
    int deaths_ = 0;
    int collisions_ = 0;
};

struct SpriteAsset
{
  std::string texture;
  std::string sound;
  std::size_t memoryKb = 0;
};

class AssetFactory
{
  public:
    std::shared_ptr<const SpriteAsset> get(const std::string& texture, const std::string& sound,
      std::size_t memoryKb)
    {
      const std::string key = texture + "|" + sound;
      auto it = cache_.find(key);
      if (it != cache_.end())
      {
        if (auto existing = it->second.lock()) { return existing; }
      }
      auto asset = std::make_shared<SpriteAsset>(SpriteAsset{texture, sound, memoryKb});
      cache_[key] = asset;
      return asset;
    }

    std::size_t uniqueAssets() const
    {
      std::size_t count = 0;
      for (const auto& [_, weak] : cache_) { if (!weak.expired()) ++count; }
      return count;
    }

  private:
    std::unordered_map<std::string, std::weak_ptr<const SpriteAsset>> cache_;
};

class IWeaponStrategy
{
  public:
    virtual ~IWeaponStrategy() = default;
    virtual std::string name() const = 0;
    virtual int damage() const = 0;
    virtual std::string attack(const std::string& attacker, const std::string& target) const = 0;
};

class SwordStrategy : public IWeaponStrategy
{
  public:
    std::string name() const override { return "Sword"; }
    int damage() const override { return 15; }
    std::string attack(const std::string& attacker, const std::string& target) const override
    {
      return attacker + " slashes " + target + " with Sword for " + std::to_string(damage()) + " dmg";
    }
};

class BowStrategy : public IWeaponStrategy
{
  public:
    std::string name() const override { return "Bow"; }
    int damage() const override { return 10; }
    std::string attack(const std::string& attacker, const std::string& target) const override
    {
      return attacker + " shoots " + target + " with Bow for " + std::to_string(damage()) + " dmg";
    }
};

class MagicStaffStrategy : public IWeaponStrategy
{
  public:
    std::string name() const override { return "MagicStaff"; }
    int damage() const override { return 22; }
    std::string attack(const std::string& attacker, const std::string& target) const override
    {
      return attacker + " casts fireball at " + target + " for " + std::to_string(damage()) + " dmg";
    }
};

class GameObject
{
  public:
    explicit GameObject(std::string name) : name_(std::move(name)) {}
    virtual ~GameObject() = default;

    const std::string& name() const { return name_; }
    virtual void update(int dtMs) { (void)dtMs; }
    virtual void render(std::ostream& os, int indent = 0) const
    {
        os << std::string(static_cast<std::size_t>(indent), ' ') << "- " << name_ << '\n';
    }
    virtual void add(std::shared_ptr<GameObject>) {}
    virtual int count() const { return 1; }

  protected:
    std::string name_;
};

class SpriteObject : public GameObject
{
  public:
    SpriteObject(std::string name, std::shared_ptr<const SpriteAsset> asset, int x, int y)
      : GameObject(std::move(name)), asset_(std::move(asset)), x_(x), y_(y) {}

    void render(std::ostream& os, int indent = 0) const override
    {
      os << std::string(static_cast<std::size_t>(indent), ' ') << "- Sprite " << name_
        << " pos=(" << x_ << "," << y_ << ") texture=" << asset_->texture
        << " sound=" << asset_->sound << '\n';
    }

  private:
    std::shared_ptr<const SpriteAsset> asset_;
    int x_ = 0;
    int y_ = 0;
};

struct Particle
{
  float x = 0, y = 0, vx = 0, vy = 0, life = 1.0f;
};

class ParticleSystem : public GameObject
{
  public:
    explicit ParticleSystem(std::string name, std::shared_ptr<const SpriteAsset> asset)
      : GameObject(std::move(name)), asset_(std::move(asset)) {}

    void emit(Particle p) { particles_.push_back(p); }

    void update(int dtMs) override
    {
      const float dt = static_cast<float>(dtMs) / 1000.0f;
      for (auto& p : particles_)
      {
        p.x += p.vx * dt;
        p.y += p.vy * dt;
        p.life -= dt;
      }
      particles_.erase(std::remove_if(particles_.begin(), particles_.end(),
        [](const Particle& p) { return p.life <= 0.0f; }),
        particles_.end());
      for (auto& child : children_) child->update(dtMs);
    }

    void render(std::ostream& os, int indent = 0) const override
    {
      os << std::string(static_cast<std::size_t>(indent), ' ') << "- ParticleSystem " << name_
        << " particles=" << particles_.size() << " asset=" << asset_->texture
        << " children=" << children_.size() << '\n';
      for (const auto& child : children_) child->render(os, indent + 2);
    }

    void add(std::shared_ptr<GameObject> child) override { children_.push_back(std::move(child)); }

    int count() const override
    {
      int total = 1;
      for (const auto& child : children_) total += child->count();
      return total;
    }

  private:
    std::shared_ptr<const SpriteAsset> asset_;
    std::vector<Particle> particles_;
    std::vector<std::shared_ptr<GameObject>> children_;
};

class CompositeGroup : public GameObject
{
  public:
    explicit CompositeGroup(std::string name) : GameObject(std::move(name)) {}

    void update(int dtMs) override { for (auto& child : children_) child->update(dtMs); }

    void render(std::ostream& os, int indent = 0) const override
    {
      os << std::string(static_cast<std::size_t>(indent), ' ') << "+ Group " << name_
        << " children=" << children_.size() << '\n';
      for (const auto& child : children_) child->render(os, indent + 2);
    }

    void add(std::shared_ptr<GameObject> child) override { children_.push_back(std::move(child)); }

    int count() const override
    {
      int total = 1;
      for (const auto& child : children_) total += child->count();
      return total;
    }

  private:
    std::vector<std::shared_ptr<GameObject>> children_;
};

class Character : public GameObject
{
  public:
    Character(std::string name, std::shared_ptr<const SpriteAsset> asset, int x, int y,
      std::shared_ptr<EventBus> bus)
      : GameObject(std::move(name)),
      asset_(std::move(asset)),
      x_(x),
      y_(y),
      bus_(std::move(bus)),
      weapon_(std::make_shared<SwordStrategy>()) {}

    void setWeapon(std::shared_ptr<IWeaponStrategy> weapon)
    {
        weapon_ = std::move(weapon);
        std::cout << "[Character] " << name_ << " switched weapon to " << weapon_->name() << '\n';
    }

    const IWeaponStrategy& weapon() const { return *weapon_; }

    void moveBy(int dx, int dy)
    {
      x_ += dx;
      y_ += dy;
      std::cout << "[Character] " << name_ << " moved to (" << x_ << "," << y_ << ")\n";
    }

    std::string attack(Character& target)
    {
      std::string result = weapon_->attack(name_, target.name());
      target.hp_ -= weapon_->damage();
      if (target.hp_ < 0) target.hp_ = 0;
      std::cout << "[Combat] " << result << "; " << target.name() << " HP=" << target.hp_ << '\n';
      if (target.hp_ == 0) { bus_->publish({"death", target.name(), "killed by " + name_}); }
      else { bus_->publish({"collision", name_, "hit " + target.name()}); }
      return result;
    }

    int hp() const { return hp_; }
    void setHpForDemo(int hp) { hp_ = hp; }
    int x() const { return x_; }
    int y() const { return y_; }

    void render(std::ostream& os, int indent = 0) const override
    {
      os << std::string(static_cast<std::size_t>(indent), ' ') << "- Character " << name_
        << " pos=(" << x_ << "," << y_ << ") hp=" << hp_ << " weapon=" << weapon_->name()
        << " texture=" << asset_->texture << '\n';
    }

  private:
    std::shared_ptr<const SpriteAsset> asset_;
    int x_ = 0;
    int y_ = 0;
    int hp_ = 100;
    std::shared_ptr<EventBus> bus_;
    std::shared_ptr<IWeaponStrategy> weapon_;
};

class ICommand
{
  public:
    virtual ~ICommand() = default;
    virtual std::string name() const = 0;
    virtual void execute() = 0;
    virtual void undo() = 0;
};

class MoveCommand : public ICommand
{
  public:
    MoveCommand(std::shared_ptr<Character> character, int dx, int dy)
      : character_(std::move(character)), dx_(dx), dy_(dy) {}

    std::string name() const override { return "Move(" + std::to_string(dx_) + "," + std::to_string(dy_) + ")"; }
    void execute() override { if (auto c = character_.lock()) c->moveBy(dx_, dy_); }
    void undo() override { if (auto c = character_.lock()) c->moveBy(-dx_, -dy_); }

  private:
    std::weak_ptr<Character> character_;
    int dx_;
    int dy_;
};

class AttackCommand : public ICommand
{
  public:
    AttackCommand(std::shared_ptr<Character> attacker, std::shared_ptr<Character> target)
      : attacker_(std::move(attacker)), target_(std::move(target)) {}

    std::string name() const override { return "Attack"; }
    void execute() override
    {
      auto a = attacker_.lock();
      auto t = target_.lock();
      if (!a || !t) return;
      prevHp_ = t->hp();
      executed_ = true;
      a->attack(*t);
    }
    void undo() override
    {
      if (!executed_) return;
      if (auto t = target_.lock())
      {
        std::cout << "[Undo] Restoring " << t->name() << " HP from " << t->hp() << " to "
          << prevHp_ << "\n";
        t->setHpForDemo(prevHp_);
      }
    }

  private:
    std::weak_ptr<Character> attacker_;
    std::weak_ptr<Character> target_;
    int prevHp_ = 0;
    bool executed_ = false;
};

class CommandQueue
{
  public:
    void enqueue(std::unique_ptr<ICommand> cmd)
    {
      std::cout << "[Queue] enqueue " << cmd->name() << '\n';
      pending_.push_back(std::move(cmd));
    }

    void processAll()
    {
      while (!pending_.empty())
      {
        auto cmd = std::move(pending_.front());
        pending_.erase(pending_.begin());
        std::cout << "[Queue] execute " << cmd->name() << '\n';
        cmd->execute();
        history_.push_back(std::move(cmd));
      }
    }

    bool undoLast()
    {
      if (history_.empty())
      {
        std::cout << "[Queue] nothing to undo\n";
        return false;
      }
      auto cmd = std::move(history_.back());
      history_.pop_back();
      std::cout << "[Queue] undo " << cmd->name() << '\n';
      cmd->undo();
      return true;
    }

    std::size_t pendingSize() const { return pending_.size(); }
    std::size_t historySize() const { return history_.size(); }

  private:
    std::vector<std::unique_ptr<ICommand>> pending_;
    std::vector<std::unique_ptr<ICommand>> history_;
};

int main()
{
  std::cout << "__ Task 3: 2D Game Core Patterns Demo __\n\n";

  AssetFactory assets;
  auto heroTex = assets.get("hero.png", "step.wav", 512);
  auto enemyTex = assets.get("enemy.png", "growl.wav", 512);
  auto fireTex = assets.get("fire.png", "burn.wav", 256);
  auto heroTex2 = assets.get("hero.png", "step.wav", 512);

  std::cout << "__ Flyweight __\n";
  std::cout << "heroTex == heroTex2: " << std::boolalpha << (heroTex.get() == heroTex2.get())
              << '\n';
  std::cout << "Unique assets in factory: " << assets.uniqueAssets() << "\n\n";

  auto bus = std::make_shared<EventBus>();
  auto logObs = std::make_shared<LoggingObserver>();
  auto achObs = std::make_shared<AchievementObserver>();
  bus->subscribe(logObs);
  bus->subscribe(achObs);

  std::cout << "__ Composite __\n";
  auto world = std::make_shared<CompositeGroup>("World");
  auto fx = std::make_shared<CompositeGroup>("FX");
  auto fire = std::make_shared<ParticleSystem>("Fire", fireTex);
  fire->emit({0, 0, 1, 2, 1.5f});
  fire->emit({0.2f, 0.1f, 0.5f, 1.5f, 0.6f});
  auto sparks = std::make_shared<ParticleSystem>("Sparks", fireTex);
  sparks->emit({0, 0, 3, 3, 0.4f});
  fire->add(sparks);
  fx->add(fire);
  world->add(fx);

  auto hero = std::make_shared<Character>("Hero", heroTex, 0, 0, bus);
  auto enemy = std::make_shared<Character>("Goblin", enemyTex, 5, 0, bus);
  world->add(hero);
  world->add(enemy);
  world->add(std::make_shared<SpriteObject>("Tree", heroTex, 10, 3));
  world->render(std::cout);
  std::cout << "Total nodes in world: " << world->count() << "\n\n";

  std::cout << "__ Composite update (dt=500ms) __\n";
  world->update(500);
  world->render(std::cout);
  std::cout << '\n';

  std::cout << "__ Strategy: runtime weapon switch __\n";
  std::cout << "Current weapon: " << hero->weapon().name() << '\n';
  hero->setWeapon(std::make_shared<BowStrategy>());
  hero->setWeapon(std::make_shared<MagicStaffStrategy>());
  std::cout << '\n';

  std::cout << "__ Command queue + undo __\n";
  CommandQueue queue;
  queue.enqueue(std::make_unique<MoveCommand>(hero, 2, 1));
  queue.enqueue(std::make_unique<MoveCommand>(hero, 3, 0));
  queue.enqueue(std::make_unique<AttackCommand>(hero, enemy));
  queue.enqueue(std::make_unique<AttackCommand>(hero, enemy));
  queue.processAll();
  std::cout << "Hero pos=(" << hero->x() << "," << hero->y() << ") Goblin HP=" << enemy->hp()
    << "\n";

  std::cout << "\n__ Undo last two commands __\n";
  queue.undoLast();
  queue.undoLast();
  std::cout << "After undo: Goblin HP=" << enemy->hp() << "\n";
  queue.undoLast();
  std::cout << "After undo move: Hero pos=(" << hero->x() << "," << hero->y() << ")\n\n";

  std::cout << "__ Observer: explicit events __\n";
  bus->publish({"collision", "Hero", "collided with wall"});
  enemy->setHpForDemo(10);
  queue.enqueue(std::make_unique<AttackCommand>(hero, enemy));
  queue.processAll();

  std::cout << "\n__ Summary __\n";
  std::cout << "Unique flyweight assets: " << assets.uniqueAssets() << '\n';
  std::cout << "LogObserver entries: " << logObs->log().size() << '\n';
  std::cout << "World nodes: " << world->count() << '\n';
  std::cout << "Demo complete.\n";
  return 0;
}
