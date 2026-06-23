#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
using namespace std;

using Date = string;
using Decimal = double;

enum class UnitStatus { OCCUPIED, VACANT };
enum class OccupancyStatus { CURRENT, PAST };

string unitStatusToString(UnitStatus s) { return s == UnitStatus::OCCUPIED ? "OCCUPIED" : "VACANT"; }
string occupancyStatusToString(OccupancyStatus s) { return s == OccupancyStatus::CURRENT ? "CURRENT" : "PAST"; }

struct AdminProfile { string id; string username; };
struct ValidationResult { bool valid; string message; };
struct QueryFilter { string keyword; };

template <typename T>
struct ApiResponse { bool success; string message; T data; };

struct QuarterCategoryCreateInput {
    string categoryName;
    string address;
    Decimal rentalPrice;
    Decimal maintenancePrice;
    Decimal penaltyPrice;
};

struct QuarterCategoryUpdateInput {
    string categoryName;
    string address;
    Decimal rentalPrice;
    Decimal maintenancePrice;
    Decimal penaltyPrice;
};

struct QuarterUnitCreateInput { string unitCode; UnitStatus status; };
struct QuarterUnitUpdateInput { string unitCode; UnitStatus status; };
struct QuarterUnitFilter { string keyword; };

struct QuarterCategoryListRequest { string keyword; };
struct QuarterCategoryCreateRequest { QuarterCategoryCreateInput input; AdminProfile actor; };
struct QuarterCategoryUpdateRequest { string categoryId; QuarterCategoryUpdateInput input; AdminProfile actor; };
struct QuarterCategoryDeleteRequest { string categoryId; AdminProfile actor; };
struct QuarterUnitListRequest { string categoryId; QuarterUnitFilter filter; };
struct QuarterUnitCreateRequest { string categoryId; QuarterUnitCreateInput input; AdminProfile actor; };
struct QuarterUnitUpdateRequest { string categoryId; string unitId; QuarterUnitUpdateInput input; AdminProfile actor; };
struct QuarterUnitDeleteRequest { string categoryId; string unitId; AdminProfile actor; };

struct QuarterCategoryDto {
    string id;
    string categoryName;
    string address;
    Decimal rentalPrice;
    Decimal maintenancePrice;
    Decimal penaltyPrice;
    int unitCount;
    int vacantUnitCount;
};

struct QuarterUnitDto { string id; string unitCode; string status; string categoryId; };

struct QuarterUnitDetailsDto {
    string id;
    string unitCode;
    string status;
    string categoryId;
    string currentOccupancyDescription;
};

struct QuarterCategorySummary {
    string categoryId;
    int totalUnitCount;
    int vacantUnitCount;
    Decimal occupancyRate;
    Decimal totalArrearsAmount;
};

class Resident {
private:
    string id;
    string fullName;
    string icNumber;
    Decimal outstandingBalance;
public:
    Resident() : id(""), fullName(""), icNumber(""), outstandingBalance(0.0) {}
    Resident(string id, string fullName, string icNumber)
        : id(id), fullName(fullName), icNumber(icNumber), outstandingBalance(0.0) {}
    string getId() const { return id; }
    string getFullName() const { return fullName; }
    string getIcNumber() const { return icNumber; }
    Decimal getOutstandingBalance() const { return outstandingBalance; }
    bool isDataIncomplete() const { return fullName.empty() || icNumber.empty(); }
};

class UnitOccupancy {
private:
    string id;
    string residentId;
    string unitId;
    Date moveInDate;
    Date moveOutDate;
    OccupancyStatus status;
public:
    UnitOccupancy()
        : id(""), residentId(""), unitId(""), moveInDate(""), moveOutDate(""), status(OccupancyStatus::CURRENT) {}
    UnitOccupancy(string id, string residentId, string unitId, Date moveInDate, Date moveOutDate)
        : id(id), residentId(residentId), unitId(unitId), moveInDate(moveInDate), moveOutDate(moveOutDate), status(OccupancyStatus::CURRENT) {}
    UnitOccupancy start(string residentId, string unitId, Date moveInDate) {
        this->residentId = residentId; this->unitId = unitId; this->moveInDate = moveInDate;
        this->moveOutDate = ""; this->status = OccupancyStatus::CURRENT; return *this;
    }
    UnitOccupancy end(Date moveOutDate, string description) {
        this->moveOutDate = moveOutDate; this->status = OccupancyStatus::PAST;
        cout << "Occupancy ended. Reason: " << description << endl; return *this;
    }
    UnitOccupancy markCurrent() { status = OccupancyStatus::CURRENT; return *this; }
    UnitOccupancy markPast() { status = OccupancyStatus::PAST; return *this; }
    bool overlaps(Date newInDate, Date newOutDate) const {
        if (status == OccupancyStatus::PAST) return false;
        if (newOutDate.empty()) return true;
        return moveInDate <= newOutDate && (moveOutDate.empty() || moveOutDate >= newInDate);
    }
    bool isCurrent(Date referenceDate) const {
        return status == OccupancyStatus::CURRENT && moveInDate <= referenceDate &&
               (moveOutDate.empty() || moveOutDate >= referenceDate);
    }
    bool isPast() const { return status == OccupancyStatus::PAST; }
    bool belongsToResident(string residentId) const { return this->residentId == residentId; }
    string getResidentId() const { return residentId; }
    string getUnitId() const { return unitId; }
    string getDescription() const {
        return residentId + " assigned to " + unitId + " from " + moveInDate +
               " (" + occupancyStatusToString(status) + ")";
    }
};

class Unit {
private:
    string id;
    string unitCode;
    UnitStatus status;
    string categoryId;
    vector<UnitOccupancy> occupancyHistory;
public:
    Unit() : id(""), unitCode(""), status(UnitStatus::VACANT), categoryId("") {}
    Unit(string id, string unitCode, UnitStatus status, string categoryId)
        : id(id), unitCode(unitCode), status(status), categoryId(categoryId) {}
    Unit rename(string newUnitCode) { unitCode = newUnitCode; return *this; }
    UnitOccupancy assignResident(string residentId, Date moveInDate, Date moveOutDate) {
        status = UnitStatus::OCCUPIED;
        UnitOccupancy occupancy("OCC_" + id + "_" + residentId, residentId, id, moveInDate, moveOutDate);
        occupancyHistory.push_back(occupancy);
        return occupancy;
    }
    UnitOccupancy vacate(Date moveOutDate, string description) {
        status = UnitStatus::VACANT;
        if (!occupancyHistory.empty()) {
            UnitOccupancy last = occupancyHistory.back().end(moveOutDate, description);
            occupancyHistory.back() = last;
            return last;
        }
        return UnitOccupancy();
    }
    UnitOccupancy getCurrentOccupancy() const {
        for (UnitOccupancy o : occupancyHistory) if (o.isCurrent("2026-06-23")) return o;
        return UnitOccupancy();
    }
    vector<UnitOccupancy> getOccupancyHistory() const { return occupancyHistory; }
    bool isOccupied() const { return status == UnitStatus::OCCUPIED; }
    bool isVacant() const { return status == UnitStatus::VACANT; }
    string getId() const { return id; }
    string getUnitCode() const { return unitCode; }
    UnitStatus getStatus() const { return status; }
    string getCategoryId() const { return categoryId; }
};

class QuarterCategory {
private:
    string id;
    string categoryName;
    string address;
    Decimal rentalPrice;
    Decimal maintenancePrice;
    Decimal penaltyPrice;
    vector<Unit> units;
public:
    QuarterCategory()
        : id(""), categoryName(""), address(""), rentalPrice(0), maintenancePrice(0), penaltyPrice(0) {}
    QuarterCategory(string id, string categoryName, string address, Decimal rentalPrice, Decimal maintenancePrice, Decimal penaltyPrice)
        : id(id), categoryName(categoryName), address(address), rentalPrice(rentalPrice), maintenancePrice(maintenancePrice), penaltyPrice(penaltyPrice) {}
    QuarterCategory updateRates(Decimal rental, Decimal maintenance, Decimal penalty) {
        rentalPrice = rental; maintenancePrice = maintenance; penaltyPrice = penalty; return *this;
    }
    QuarterCategory updateDetails(string categoryName, string address) {
        this->categoryName = categoryName; this->address = address; return *this;
    }
    Unit addUnit(string unitCode) {
        Unit unit("UNIT_" + to_string(units.size() + 1), unitCode, UnitStatus::VACANT, id);
        units.push_back(unit); return unit;
    }
    bool removeUnit(string unitId) {
        for (int i = 0; i < (int)units.size(); i++) {
            if (units[i].getId() == unitId && units[i].isVacant()) { units.erase(units.begin() + i); return true; }
        }
        return false;
    }
    Decimal calculateOccupancyRate() const {
        if (units.empty()) return 0.0;
        int occupied = 0; for (Unit u : units) if (u.isOccupied()) occupied++;
        return (static_cast<Decimal>(occupied) / units.size()) * 100.0;
    }
    bool canDelete() const { for (Unit u : units) if (u.isOccupied()) return false; return true; }
    string getDisplayName() const { return categoryName + " - " + address; }
    int calculateUnitCount(vector<Unit> unitList) const {
        int count = 0; for (Unit u : unitList) if (u.getCategoryId() == id) count++; return count;
    }
    Decimal calculateArrearsAmount(vector<Decimal> summaries) const {
        Decimal total = 0; for (Decimal amount : summaries) total += amount; return total;
    }
    string getId() const { return id; }
    string getCategoryName() const { return categoryName; }
    string getAddress() const { return address; }
    Decimal getRentalPrice() const { return rentalPrice; }
    Decimal getMaintenancePrice() const { return maintenancePrice; }
    Decimal getPenaltyPrice() const { return penaltyPrice; }
    vector<Unit> getUnits() const { return units; }
};

template <typename T>
class IRepository {
public:
    virtual T findById(string id) = 0;
    virtual vector<T> findMany(QueryFilter filter) = 0;
    virtual T create(T input) = 0;
    virtual T update(string id, T input) = 0;
    virtual bool remove(string id) = 0;
    virtual ~IRepository() {}
};

class ResidentRepository : public IRepository<Resident> {
private:
    vector<Resident> residents;
public:
    ResidentRepository() {
        residents.push_back(Resident("RES_001", "Ali Bin Abu", "660101011111"));
        residents.push_back(Resident("RES_002", "Siti Binti Ahmad", "900505015555"));
    }
    Resident findById(string id) override { for (Resident r : residents) if (r.getId() == id) return r; return Resident(); }
    vector<Resident> findMany(QueryFilter filter) override {
        vector<Resident> result; for (Resident r : residents) if (filter.keyword.empty() || r.getFullName().find(filter.keyword) != string::npos || r.getIcNumber().find(filter.keyword) != string::npos) result.push_back(r); return result;
    }
    Resident create(Resident input) override { residents.push_back(input); return input; }
    Resident update(string id, Resident input) override { for (int i=0;i<(int)residents.size();i++) if (residents[i].getId()==id) { residents[i]=input; return residents[i]; } return Resident(); }
    bool remove(string id) override { for (int i=0;i<(int)residents.size();i++) if (residents[i].getId()==id) { residents.erase(residents.begin()+i); return true; } return false; }
    Resident findByIcNumber(string icNumber) { for (Resident r : residents) if (r.getIcNumber()==icNumber) return r; return Resident(); }
    vector<Resident> findAvailableResidents(QueryFilter filter) { return findMany(filter); }
    Resident findWithDetails(string id) { return findById(id); }
    Resident save(Resident resident) { return findById(resident.getId()).getId().empty() ? create(resident) : update(resident.getId(), resident); }
};

class QuarterRepository : public IRepository<QuarterCategory> {
private:
    vector<QuarterCategory> categories;
    vector<Unit> units;
public:
    QuarterRepository() {
        categories.push_back(QuarterCategory("QCAT_001", "Class A Quarters", "Block A", 300.00, 50.00, 100.00));
        units.push_back(Unit("UNIT_001", "A-01", UnitStatus::VACANT, "QCAT_001"));
        units.push_back(Unit("UNIT_002", "A-02", UnitStatus::OCCUPIED, "QCAT_001"));
    }
    QuarterCategory findById(string id) override { for (QuarterCategory c : categories) if (c.getId()==id) return c; return QuarterCategory(); }
    vector<QuarterCategory> findMany(QueryFilter filter) override {
        vector<QuarterCategory> result; for (QuarterCategory c : categories) if (filter.keyword.empty() || c.getCategoryName().find(filter.keyword)!=string::npos || c.getAddress().find(filter.keyword)!=string::npos) result.push_back(c); return result;
    }
    QuarterCategory create(QuarterCategory input) override { categories.push_back(input); return input; }
    QuarterCategory update(string id, QuarterCategory input) override { for (int i=0;i<(int)categories.size();i++) if (categories[i].getId()==id) { categories[i]=input; return categories[i]; } return QuarterCategory(); }
    bool remove(string id) override { for (int i=0;i<(int)categories.size();i++) if (categories[i].getId()==id) { categories.erase(categories.begin()+i); return true; } return false; }
    QuarterCategory findCategoryByNameAddress(string name, string address) { for (QuarterCategory c:categories) if (c.getCategoryName()==name && c.getAddress()==address) return c; return QuarterCategory(); }
    Unit findUnitByCategoryAndCode(string categoryId, string unitCode) { for (Unit u:units) if (u.getCategoryId()==categoryId && u.getUnitCode()==unitCode) return u; return Unit(); }
    QuarterCategory findCategoryWithUnits(string categoryId) { return findById(categoryId); }
    QuarterCategory saveCategory(QuarterCategory c) { return findById(c.getId()).getId().empty() ? create(c) : update(c.getId(), c); }
    Unit saveUnit(Unit unit) { for (int i=0;i<(int)units.size();i++) if (units[i].getId()==unit.getId()) { units[i]=unit; return units[i]; } units.push_back(unit); return unit; }
    bool deleteUnit(string unitId) { for (int i=0;i<(int)units.size();i++) if (units[i].getId()==unitId) { units.erase(units.begin()+i); return true; } return false; }
    vector<Unit> findUnitsByCategory(string categoryId, QuarterUnitFilter filter) {
        vector<Unit> result; for (Unit u:units) if (u.getCategoryId()==categoryId && (filter.keyword.empty() || u.getUnitCode().find(filter.keyword)!=string::npos)) result.push_back(u); return result;
    }
    Unit findUnitById(string unitId) { for (Unit u:units) if (u.getId()==unitId) return u; return Unit(); }
};

template <typename T, typename D>
class EntityMapper {
public:
    virtual D toDto(T entity) = 0;
    virtual vector<D> toDtoList(vector<T> entities) = 0;
    virtual ~EntityMapper() {}
};

class QuarterMapper : public EntityMapper<QuarterCategory, QuarterCategoryDto> {
public:
    QuarterCategoryDto toDto(QuarterCategory e) override { return { e.getId(), e.getCategoryName(), e.getAddress(), e.getRentalPrice(), e.getMaintenancePrice(), e.getPenaltyPrice(), 0, 0 }; }
    vector<QuarterCategoryDto> toDtoList(vector<QuarterCategory> entities) override { vector<QuarterCategoryDto> result; for (QuarterCategory e:entities) result.push_back(toDto(e)); return result; }
    QuarterUnitDto toUnitDto(Unit u) { return { u.getId(), u.getUnitCode(), unitStatusToString(u.getStatus()), u.getCategoryId() }; }
    vector<QuarterUnitDto> toUnitDtoList(vector<Unit> units) { vector<QuarterUnitDto> result; for (Unit u:units) result.push_back(toUnitDto(u)); return result; }
    QuarterUnitDetailsDto toUnitDetailsDto(Unit unit) {
        UnitOccupancy occ = unit.getCurrentOccupancy();
        string desc = occ.getResidentId().empty() ? "No current occupancy" : occ.getDescription();
        return { unit.getId(), unit.getUnitCode(), unitStatusToString(unit.getStatus()), unit.getCategoryId(), desc };
    }
};

class QuarterCategoryService {
private:
    QuarterRepository& quarterRepository;
public:
    QuarterCategoryService(QuarterRepository& repo) : quarterRepository(repo) {}
    vector<QuarterCategory> getQuarterCategories() { return quarterRepository.findMany({""}); }
    ValidationResult validateCategoryInput(QuarterCategoryCreateInput input) {
        if (input.categoryName.empty() || input.address.empty()) return {false, "Category name and address are required."};
        if (input.rentalPrice < 0 || input.maintenancePrice < 0 || input.penaltyPrice < 0) return {false, "Price values cannot be negative."};
        return {true, "Quarter category input is valid."};
    }
    QuarterCategory createQuarterCategory(QuarterCategoryCreateInput input, AdminProfile actor) {
        ValidationResult v = validateCategoryInput(input); if (!v.valid) { cout << v.message << endl; return QuarterCategory(); }
        QuarterCategory c("QCAT_" + input.categoryName, input.categoryName, input.address, input.rentalPrice, input.maintenancePrice, input.penaltyPrice);
        cout << "Quarter category created by admin: " << actor.username << endl; return quarterRepository.saveCategory(c);
    }
    QuarterCategory updateQuarterCategory(string id, QuarterCategoryUpdateInput input, AdminProfile actor) {
        QuarterCategory c = quarterRepository.findById(id); if (c.getId().empty()) return QuarterCategory();
        c.updateDetails(input.categoryName, input.address).updateRates(input.rentalPrice, input.maintenancePrice, input.penaltyPrice);
        cout << "Quarter category updated by admin: " << actor.username << endl; return quarterRepository.saveCategory(c);
    }
    bool deleteQuarterCategory(string id, AdminProfile actor) {
        QuarterCategory c = quarterRepository.findById(id); if (c.getId().empty() || !c.canDelete()) return false;
        cout << "Quarter category deleted by admin: " << actor.username << endl; return quarterRepository.remove(id);
    }
    QuarterCategorySummary buildQuarterCategorySummary(string categoryId) {
        vector<Unit> units = quarterRepository.findUnitsByCategory(categoryId, {""}); int vacant = 0;
        for (Unit u:units) if (u.isVacant()) vacant++;
        Decimal rate = units.empty() ? 0.0 : (static_cast<Decimal>(units.size() - vacant) / units.size()) * 100.0;
        return { categoryId, (int)units.size(), vacant, rate, 0.0 };
    }
};

class QuarterUnitService {
private:
    QuarterRepository& quarterRepository;
    ResidentRepository& residentRepository;
public:
    QuarterUnitService(QuarterRepository& qRepo, ResidentRepository& rRepo) : quarterRepository(qRepo), residentRepository(rRepo) {}
    vector<Unit> getQuarterUnits(string categoryId, QuarterUnitFilter filter) { return quarterRepository.findUnitsByCategory(categoryId, filter); }
    QuarterUnitDetailsDto getQuarterUnitDetails(string unitId) { QuarterMapper mapper; return mapper.toUnitDetailsDto(quarterRepository.findUnitById(unitId)); }
    Unit createQuarterUnit(string categoryId, QuarterUnitCreateInput input, AdminProfile actor) {
        if (!quarterRepository.findUnitByCategoryAndCode(categoryId, input.unitCode).getId().empty()) { cout << "Unit code already exists in this category." << endl; return Unit(); }
        Unit unit("UNIT_" + input.unitCode, input.unitCode, input.status, categoryId);
        cout << "Quarter unit created by admin: " << actor.username << endl; return quarterRepository.saveUnit(unit);
    }
    Unit updateQuarterUnit(string categoryId, string unitId, QuarterUnitUpdateInput input, AdminProfile actor) {
        Unit unit = quarterRepository.findUnitById(unitId); if (unit.getId().empty()) return Unit();
        unit.rename(input.unitCode); cout << "Quarter unit updated by admin: " << actor.username << endl; return quarterRepository.saveUnit(unit);
    }
    bool deleteQuarterUnit(string categoryId, string unitId, AdminProfile actor) {
        Unit unit = quarterRepository.findUnitById(unitId); if (unit.getId().empty() || unit.isOccupied()) return false;
        cout << "Quarter unit deleted by admin: " << actor.username << endl; return quarterRepository.deleteUnit(unitId);
    }
    bool checkOccupancyConflict(string unitId, string residentId, Date moveInDate, Date moveOutDate) {
        Unit unit = quarterRepository.findUnitById(unitId); if (unit.getId().empty() || unit.isOccupied()) return true;
        Resident resident = residentRepository.findById(residentId); if (resident.getId().empty()) return true;
        return false;
    }
    UnitOccupancy assignResidentToUnit(string unitId, string residentId, Date moveInDate, Date moveOutDate) {
        if (checkOccupancyConflict(unitId, residentId, moveInDate, moveOutDate)) { cout << "Occupancy conflict detected." << endl; return UnitOccupancy(); }
        Unit unit = quarterRepository.findUnitById(unitId);
        UnitOccupancy occupancy = unit.assignResident(residentId, moveInDate, moveOutDate);
        quarterRepository.saveUnit(unit); return occupancy;
    }
};

// =====================================================
// Structural Pattern: Facade
// Class: QuarterManagementFacade
// =====================================================

class QuarterManagementFacade {
private:
    QuarterCategoryService& categoryService;
    QuarterUnitService& unitService;
public:
    QuarterManagementFacade(QuarterCategoryService& c, QuarterUnitService& u) : categoryService(c), unitService(u) {}
    QuarterCategory createCategory(QuarterCategoryCreateInput input, AdminProfile actor) { return categoryService.createQuarterCategory(input, actor); }
    QuarterCategory updateCategory(string id, QuarterCategoryUpdateInput input, AdminProfile actor) { return categoryService.updateQuarterCategory(id, input, actor); }
    bool deleteCategory(string id, AdminProfile actor) { return categoryService.deleteQuarterCategory(id, actor); }
    Unit createUnit(string categoryId, QuarterUnitCreateInput input, AdminProfile actor) { return unitService.createQuarterUnit(categoryId, input, actor); }
    Unit updateUnit(string categoryId, string unitId, QuarterUnitUpdateInput input, AdminProfile actor) { return unitService.updateQuarterUnit(categoryId, unitId, input, actor); }
    bool deleteUnit(string categoryId, string unitId, AdminProfile actor) { return unitService.deleteQuarterUnit(categoryId, unitId, actor); }
};

class QuarterController {
private:
    QuarterManagementFacade& facade;
    QuarterCategoryService& categoryService;
    QuarterUnitService& unitService;
    QuarterMapper mapper;
public:
    QuarterController(QuarterManagementFacade& f, QuarterCategoryService& c, QuarterUnitService& u) : facade(f), categoryService(c), unitService(u) {}
    ApiResponse<vector<QuarterCategoryDto>> getQuarterCategories(QuarterCategoryListRequest request) {
        return { true, "Quarter categories retrieved successfully.", mapper.toDtoList(categoryService.getQuarterCategories()) };
    }
    ApiResponse<QuarterCategoryDto> postQuarterCategory(QuarterCategoryCreateRequest request) {
        QuarterCategory c = facade.createCategory(request.input, request.actor);
        if (c.getId().empty()) return { false, "Quarter category creation failed.", QuarterCategoryDto() };
        return { true, "Quarter category created successfully.", mapper.toDto(c) };
    }
    ApiResponse<QuarterCategoryDto> patchQuarterCategory(QuarterCategoryUpdateRequest request) {
        QuarterCategory c = facade.updateCategory(request.categoryId, request.input, request.actor);
        if (c.getId().empty()) return { false, "Quarter category update failed.", QuarterCategoryDto() };
        return { true, "Quarter category updated successfully.", mapper.toDto(c) };
    }
    ApiResponse<bool> deleteQuarterCategory(QuarterCategoryDeleteRequest request) {
        bool deleted = facade.deleteCategory(request.categoryId, request.actor);
        return { deleted, deleted ? "Quarter category deleted successfully." : "Quarter category deletion failed.", deleted };
    }
    ApiResponse<vector<QuarterUnitDto>> getUnits(QuarterUnitListRequest request) {
        return { true, "Quarter units retrieved successfully.", mapper.toUnitDtoList(unitService.getQuarterUnits(request.categoryId, request.filter)) };
    }
    ApiResponse<QuarterUnitDto> postUnit(QuarterUnitCreateRequest request) {
        Unit unit = facade.createUnit(request.categoryId, request.input, request.actor);
        if (unit.getId().empty()) return { false, "Quarter unit creation failed.", QuarterUnitDto() };
        return { true, "Quarter unit created successfully.", mapper.toUnitDto(unit) };
    }
};

int main() {
    cout << fixed << setprecision(2);

    AdminProfile admin = { "ADMIN_001", "quarterAdmin" };
    QuarterRepository quarterRepository;
    ResidentRepository residentRepository;
    QuarterCategoryService categoryService(quarterRepository);
    QuarterUnitService unitService(quarterRepository, residentRepository);
    QuarterManagementFacade facade(categoryService, unitService);
    QuarterController controller(facade, categoryService, unitService);

    QuarterCategoryCreateRequest createCategoryRequest = {
        { "Class B Quarters", "Block B", 250.00, 40.00, 80.00 }, admin
    };

    ApiResponse<QuarterCategoryDto> categoryResponse = controller.postQuarterCategory(createCategoryRequest);
    cout << "\n===== Create Quarter Category =====" << endl;
    cout << categoryResponse.message << endl;
    cout << "Category: " << categoryResponse.data.categoryName << endl;
    cout << "Address: " << categoryResponse.data.address << endl;
    cout << "Rental Price: RM " << categoryResponse.data.rentalPrice << endl;

    QuarterUnitCreateRequest createUnitRequest = {
        categoryResponse.data.id,
        { "B-01", UnitStatus::VACANT },
        admin
    };

    ApiResponse<QuarterUnitDto> unitResponse = controller.postUnit(createUnitRequest);
    cout << "\n===== Create Quarter Unit =====" << endl;
    cout << unitResponse.message << endl;
    cout << "Unit Code: " << unitResponse.data.unitCode << endl;
    cout << "Unit Status: " << unitResponse.data.status << endl;

    ApiResponse<vector<QuarterUnitDto>> listUnitResponse = controller.getUnits({ categoryResponse.data.id, { "" } });
    cout << "\n===== Quarter Unit List =====" << endl;
    for (QuarterUnitDto unit : listUnitResponse.data) {
        cout << "Unit: " << unit.unitCode << " | Status: " << unit.status << endl;
    }

    UnitOccupancy occupancy = unitService.assignResidentToUnit(unitResponse.data.id, "RES_001", "2026-06-01", "");
    cout << "\n===== Assign Resident To Unit =====" << endl;
    if (occupancy.getResidentId().empty()) {
        cout << "Resident assignment failed." << endl;
    } else {
        cout << "Resident assignment successful." << endl;
        cout << occupancy.getDescription() << endl;
    }

    ApiResponse<vector<QuarterCategoryDto>> categoryListResponse = controller.getQuarterCategories({ "" });
    cout << "\n===== Quarter Category List =====" << endl;
    for (QuarterCategoryDto category : categoryListResponse.data) {
        cout << "Category: " << category.categoryName
             << " | Address: " << category.address
             << " | Rental: RM " << category.rentalPrice << endl;
    }

    return 0;
}
