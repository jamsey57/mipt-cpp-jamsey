#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <iomanip>
#include <exception>
#include <cctype>
class FleetError : public std::exception {
public:
  FleetError(std::string message) : message_(message) {}
  const char* what() const noexcept {return message_.c_str();}
private:
  std::string message_;
};
class InspectionError : public FleetError {
public:
  InspectionError() : FleetError("inspection failed") {}
};
class DataError : public FleetError {
public:
  DataError(std::string message) : FleetError(message) {}
};

struct LogEntry {
  std::string date;
  std::string time;
  std::string action;
};

std::ostream& operator<<(std::ostream& out, const LogEntry& entry) {
  return out << "[" << entry.date << " " << entry.time << "] " << entry.action;
}

template <typename T, int Cap>
class Registry {
public:
  void add(const T& thing) {
    if (things_.size() >= Cap) {
      throw DataError("registry is full");
    }
    things_.push_back(thing);
  }
  int size() const {
    return things_.size();
  }
  const T& operator[](int index) const {
    return things_[index];
  }
private:
  std::vector<T> things_;
};
template <typename T, int Cap>
std::ostream& operator<<(std::ostream& out, const Registry<T, Cap>& reg) {
  for (int i = 0; i < reg.size(); i++) {
    out << reg[i] << "\n";
  }
  return out;
}

struct ModelInfo {
  std::string category;
  int max_speed;
};

class InventoryItem {
public:
  virtual ~InventoryItem() = default;
  std::shared_ptr<int> lifetime() const {
    return life_;
  }
  virtual std::string inventoryId() const = 0;
  virtual std::string name() const = 0;
  virtual std::string describe() const = 0;
  virtual double energyUse() const {return 0.0;}
  virtual bool forNovice() const {return false;}
  virtual void setCode(const std::string&) {
    throw DataError("not a smart lock");
  }
private:
  std::shared_ptr<int> life_ = std::make_shared<int>(0);
};

class EnergyConsumer {
public:
  virtual ~EnergyConsumer() = default;
  virtual double dailyEnergy() const = 0;
};

class Transport : public InventoryItem {
public:
  Transport(std::string id, std::string name, int score, std::shared_ptr<ModelInfo> model_info) :
    id_(id), name_(name), score_(score), model_info_(model_info) {}
  virtual std::string inventoryId() const {return id_;}
  virtual std::string name() const {return name_;}
  int score() const {return score_;}
  virtual bool forNovice() const {return score_ >= 6;}
  std::shared_ptr<ModelInfo> model() const { return model_info_; }
private:
  std::string id_;
  std::string name_;
  int score_;
  std::shared_ptr<ModelInfo> model_info_;
};

class Bike : public Transport {
public:
  Bike(std::string id, std::string name, int score, std::shared_ptr<ModelInfo> model_info) :
    Transport(id, name, score, model_info) {}
  virtual std::string describe() const {return "bike";}
};
class Scooter : public Transport, public EnergyConsumer {
public:
  Scooter(std::string id, std::string name, double energy, int score, std::shared_ptr<ModelInfo> model_info) :
    Transport(id, name, score, model_info),
    energy_(energy) {}
  virtual double dailyEnergy() const {return energy_;}
  virtual std::string describe() const {return "scooter";}
  virtual double energyUse() const {return dailyEnergy();}
private:
  double energy_;
};
class ElectroBike : public Bike, public EnergyConsumer {
public:
  ElectroBike(std::string id, std::string name, double energy, int score, std::shared_ptr<ModelInfo> model_info) :
    Bike(id, name, score, model_info),
    energy_(energy) {}
  virtual double dailyEnergy() const {return energy_;}
  virtual std::string describe() const {return "electrobike";}
  virtual double energyUse() const {return dailyEnergy();}
private:
  double energy_;
};

class Helmet : public InventoryItem {
public:
  Helmet(std::string id, std::string name) : id_(id), name_(name) {}
  virtual std::string inventoryId() const {return id_;}
  virtual std::string name() const {return name_;}
  virtual std::string describe() const {return "helmet";}
private:
  std::string id_;
  std::string name_;
};
class DockStation : public InventoryItem, public EnergyConsumer {
public:
  DockStation(std::string id, std::string name, double energy) :
    id_(id),
    name_(name),
    energy_(energy) {}
  virtual std::string inventoryId() const {return id_;}
  virtual std::string name() const {return name_;}
  virtual std::string describe() const {return "dock station";}
  virtual double dailyEnergy() const {return energy_;}
  virtual double energyUse() const {return dailyEnergy();}
private:
  std::string id_;
  std::string name_;
  double energy_;
};
class SmartLock : public InventoryItem, public EnergyConsumer {
public:
  SmartLock(std::string id, std::string name, double energy) :
    id_(id),
    name_(name),
    energy_(energy),
    code_("0000") {}
  virtual std::string inventoryId() const {return id_;}
  virtual std::string name() const {return name_;}
  virtual std::string describe() const {return "smart lock";}
  virtual double dailyEnergy() const {return energy_;}
  virtual double energyUse() const {return dailyEnergy();}
  std::string code() const {return code_;}
  virtual void setCode(const std::string& new_code) {
    if (new_code.size() != 4) {
      throw DataError("invalid code");
    }
    code_ = new_code;
  }
private:
  std::string id_;
  std::string name_;
  double energy_;
  std::string code_;
};

std::ostream& operator<<(std::ostream& out, const InventoryItem& thing) {
  return out << thing.inventoryId() << " " << thing.name();
}
bool operator<(const InventoryItem& thing1, const InventoryItem& thing2) {
  return thing1.inventoryId() < thing2.inventoryId();
}

void inspection(const std::string& name) {
  std::string copy_name = name;
  for (char& c : copy_name) {
    c = std::tolower(c);
  }
  if (copy_name.find("broken") != std::string::npos) {
    throw InspectionError();
  }
}
void checkEnergy(const double energy) {
  if (energy < 0) {
    throw DataError("impossible energy");
  }
}
void checkScore(const int score) {
  if (score < 1 || score > 10) {
    throw DataError("impossible score");
  }
}

class Fleet {
public:
  void add(std::unique_ptr<InventoryItem> thing, const std::string& date, const std::string& time) {
    admin_log_.add(LogEntry{date, time, "added " + thing->inventoryId()});
    park_.push_back(std::move(thing));
  }
  void list() {
    std::sort(park_.begin(), park_.end(),
              [](const std::unique_ptr<InventoryItem>& thing1,
                 const std::unique_ptr<InventoryItem>& thing2) {
              return *thing1 < *thing2;
              });
    for (const auto& thing : park_) {
      std::cout << *thing << "\n";
    }
  }
  void energy() {
    double ans = 0;
    for (const auto& thing : park_) {
      ans += thing->energyUse();
    }
    std::cout << std::fixed << std::setprecision(1) << ans << "\n";
  }
  void count() const {
    std::cout << park_.size() << "\n";
  }
  void novice() {
    for (const auto& thing : park_) {
      if (thing->forNovice()) {
        std::cout << *thing << "\n";
      }
    }
  }
  bool remove(const std::string& id, const std::string& date, const std::string& time) {
    for (int i = 0; i < park_.size(); i++) {
      if (park_[i]->inventoryId() == id) {
        admin_log_.add(LogEntry{date, time, "removed " + id});
        park_.erase(park_.begin() + i);
        return true;
      }
    }
    return false;
  }
  void focus(const std::string& id) {
    for (const auto& thing : park_) {
      if (thing->inventoryId() == id) {
        showcase_log_.add(id);
        focus_ = thing->lifetime();
        focus_thing_ = thing.get();
        return;
      }
    }
  }
  void show_focus() {
    if (auto thing = focus_.lock()) {
      std::cout << *focus_thing_ << "\n";
    } else {
      focus_thing_ = nullptr;
      std::cout << "expired" << "\n";
    }
  }
  void admin_history() const {
    std::cout << admin_log_;
  }
  void showcase_history() const {
    std::cout << showcase_log_;
  }
  void update_code(const std::string& id, const std::string& new_code) {
    for (const auto& thing : park_) {
      if (thing->inventoryId() == id) {
        thing->setCode(new_code);
        return;
      }
    }
    throw DataError("lock not found");
  }
private:
  std::vector<std::unique_ptr<InventoryItem>> park_;
  std::weak_ptr<int> focus_;
  InventoryItem* focus_thing_ = nullptr;
  Registry<LogEntry, 100> admin_log_;
  Registry<std::string, 50> showcase_log_;
};
int main() {
  Fleet fleet;
  std::string c;
  auto bike_info = std::make_shared<ModelInfo>(ModelInfo{"bike", 25});
  auto scooter_info = std::make_shared<ModelInfo>(ModelInfo{"scooter", 20});
  auto electrobike_info = std::make_shared<ModelInfo>(ModelInfo{"electrobike", 35});
  while (std::cin >> c) {
    try {
      if (c == "quit") {
        break;
      } else if (c == "list") {
        fleet.list();
      } else if (c == "energy") {
        fleet.energy();
      } else if (c == "count") {
        fleet.count();
      } else if (c == "novice") {
        fleet.novice();
      } else if (c == "remove") {
        std::string id, date, time;
        std::cin >> id >> date >> time;
        fleet.remove(id, date, time);
      } else if (c == "add") {
        std::string type, date, time;
        std::cin >> type >> date >> time;
        if (type == "helmet") {
          std::string id, name;
          std::cin >> id >> name;
          fleet.add(std::make_unique<Helmet>(id, name), date, time);
        } else if (type == "dock") {
          std::string id, name;
          double energy;
          std::cin >> id >> name >> energy;
          checkEnergy(energy);
          fleet.add(std::make_unique<DockStation>(id, name, energy), date, time);
        }  else if (type == "ebike") {
          std::string id, name;
          double energy;
          int score;
          std::cin >> id >> name >> energy >> score;
          inspection(name);
          checkEnergy(energy);
          checkScore(score);
          fleet.add(std::make_unique<ElectroBike>(id, name, energy, score, electrobike_info), date, time);
        } else if (type == "bike") {
          std::string id, name;
          int score;
          std::cin >> id >> name >> score;
          inspection(name);
          checkScore(score);
          fleet.add(std::make_unique<Bike>(id, name, score, bike_info), date, time);
        } else if (type == "scooter") {
          std::string id, name;
          double energy;
          int score;
          std::cin >> id >> name >> energy >> score;
          inspection(name);
          checkEnergy(energy);
          checkScore(score);
          fleet.add(std::make_unique<Scooter>(id, name, energy, score, scooter_info), date, time);
        } else if (type == "lock") {
          std::string id, name;
          double energy;
          std::cin >> id >> name >> energy;
          checkEnergy(energy);
          fleet.add(std::make_unique<SmartLock>(id, name, energy), date, time);
        }
      } else if (c == "focus") {
        std::string id;
        std::cin >> id;
        fleet.focus(id);
      } else if (c == "show_focus") {
        fleet.show_focus();
      } else if (c == "admin_history") {
        fleet.admin_history();
      } else if (c == "showcase_history") {
        fleet.showcase_history();
      } else if (c == "update_code") {
        std::string id, new_code;
        std::cin >> id >> new_code;
        fleet.update_code(id, new_code);
      }
    } catch(const FleetError& e) {
      std::cout << "error: " << e.what() << "\n";
    }
  }
  return 0;
}
