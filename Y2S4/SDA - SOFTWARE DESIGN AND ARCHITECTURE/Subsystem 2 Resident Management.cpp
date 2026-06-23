#include <iostream>
#include <string>
#include <vector>

using namespace std;

// =====================================================
// Enumerations
// =====================================================

enum class ResidentStatus {
    AKTIF,
    TIDAK_LAYAK,
    PENCEN_MENDATANG,
    DATA_TIDAK_LENGKAP
};

string statusToString(ResidentStatus status) {
    switch (status) {
        case ResidentStatus::AKTIF:
            return "AKTIF";
        case ResidentStatus::TIDAK_LAYAK:
            return "TIDAK_LAYAK";
        case ResidentStatus::PENCEN_MENDATANG:
            return "PENCEN_MENDATANG";
        case ResidentStatus::DATA_TIDAK_LENGKAP:
            return "DATA_TIDAK_LENGKAP";
        default:
            return "UNKNOWN";
    }
}

// =====================================================
// Basic Models and DTOs
// =====================================================

struct AdminProfile {
    string id;
    string username;
};

struct UnitOccupancy {
    string unitId;
    bool currentlyOccupied;
};

struct ArrearsSummary {
    double totalOutstanding;
};

struct ResidentCreateInput {
    string fullName;
    string icNumber;
    string phone;
    string email;
    string position;
    string department;
    string serviceLevel;
};

struct ResidentUpdateInput {
    string phone;
    string email;
    string position;
    string department;
    string serviceLevel;
};

struct ResidentDto {
    string id;
    string fullName;
    string icNumber;
    string status;
};

struct ResidentDetailsDto {
    string id;
    string fullName;
    string icNumber;
    string phone;
    string email;
    string position;
    string department;
    string serviceLevel;
    string status;
    double outstandingBalance;
};

struct ResidentFilter {
    string keyword;
};

struct AvailableResidentFilter {
    string department;
};

struct QueryFilter {
    string keyword;
};

template <typename T>
struct PaginatedResult {
    vector<T> items;
    int page;
    int limit;
    int total;
};

template <typename T>
struct ApiResponse {
    bool success;
    string message;
    T data;
};

struct ResidentListRequest {
    ResidentFilter filter;
    int page;
    int limit;
};

struct ResidentCreateRequest {
    ResidentCreateInput input;
    AdminProfile actor;
};

struct ResidentUpdateRequest {
    string id;
    ResidentUpdateInput input;
    AdminProfile actor;
};

struct ResidentDetailRequest {
    string id;
};

struct ResidentDeleteRequest {
    string id;
    AdminProfile actor;
};

// =====================================================
// Domain Entity
// Class: Resident
// =====================================================

class Resident {
private:
    string id;
    string fullName;
    string icNumber;
    string phone;
    string email;
    string position;
    string department;
    string serviceLevel;
    ResidentStatus status;
    bool hasCurrentOccupancy;
    double outstandingBalance;

public:
    Resident() {
        id = "";
        fullName = "";
        icNumber = "";
        phone = "";
        email = "";
        position = "";
        department = "";
        serviceLevel = "";
        status = ResidentStatus::DATA_TIDAK_LENGKAP;
        hasCurrentOccupancy = false;
        outstandingBalance = 0.0;
    }

    Resident(string id, ResidentCreateInput input) {
        this->id = id;
        this->fullName = input.fullName;
        this->icNumber = input.icNumber;
        this->phone = input.phone;
        this->email = input.email;
        this->position = input.position;
        this->department = input.department;
        this->serviceLevel = input.serviceLevel;
        this->status = ResidentStatus::DATA_TIDAK_LENGKAP;
        this->hasCurrentOccupancy = false;
        this->outstandingBalance = 0.0;
    }

    string getId() const {
        return id;
    }

    string getFullName() const {
        return fullName;
    }

    string getIcNumber() const {
        return icNumber;
    }

    string getPhone() const {
        return phone;
    }

    string getEmail() const {
        return email;
    }

    string getPosition() const {
        return position;
    }

    string getDepartment() const {
        return department;
    }

    string getServiceLevel() const {
        return serviceLevel;
    }

    ResidentStatus getStatus() const {
        return status;
    }

    double getOutstandingBalance() const {
        return outstandingBalance;
    }

    void setCurrentOccupancy(bool occupied) {
        hasCurrentOccupancy = occupied;
    }

    void setOutstandingBalance(double amount) {
        outstandingBalance = amount;
    }

    Resident updateDetails(ResidentUpdateInput input) {
        phone = input.phone;
        email = input.email;
        position = input.position;
        department = input.department;
        serviceLevel = input.serviceLevel;
        return *this;
    }

    Resident changeStatus(ResidentStatus newStatus, string description) {
        status = newStatus;
        cout << "Status changed to " << statusToString(newStatus)
             << ". Reason: " << description << endl;
        return *this;
    }

    int calculateAge(int referenceYear) const {
        if (icNumber.length() < 2) {
            return 0;
        }

        int yearPrefix = stoi(icNumber.substr(0, 2));
        int birthYear;

        if (yearPrefix <= 30) {
            birthYear = 2000 + yearPrefix;
        } else {
            birthYear = 1900 + yearPrefix;
        }

        return referenceYear - birthYear;
    }

    UnitOccupancy getCurrentOccupancy() const {
        return { "UNIT-A-01", hasCurrentOccupancy };
    }

    double getOutstandingBalanceValue() const {
        return outstandingBalance;
    }

    bool isEligibleForBilling() const {
        return status == ResidentStatus::AKTIF ||
               status == ResidentStatus::PENCEN_MENDATANG;
    }

    bool isDataIncomplete() const {
        return fullName.empty() || icNumber.empty();
    }

    ArrearsSummary getArrearsSummary() const {
        return { outstandingBalance };
    }
};

// =====================================================
// Behavioral Pattern: Strategy
// Interface: IResidentStatusStrategy
// =====================================================

class IResidentStatusStrategy {
public:
    virtual ResidentStatus resolveStatus(Resident resident, int referenceYear) = 0;
    virtual string getReason(Resident resident, int referenceYear) = 0;
    virtual ~IResidentStatusStrategy() {}
};

// =====================================================
// Concrete Strategy: ActiveResidentStatusStrategy
// =====================================================

class ActiveResidentStatusStrategy : public IResidentStatusStrategy {
public:
    ResidentStatus resolveStatus(Resident resident, int referenceYear) override {
        return ResidentStatus::AKTIF;
    }

    string getReason(Resident resident, int referenceYear) override {
        return "Resident data is complete and the resident is below pension warning age.";
    }
};

// =====================================================
// Concrete Strategy: UpcomingPensionStatusStrategy
// =====================================================

class UpcomingPensionStatusStrategy : public IResidentStatusStrategy {
public:
    ResidentStatus resolveStatus(Resident resident, int referenceYear) override {
        return ResidentStatus::PENCEN_MENDATANG;
    }

    string getReason(Resident resident, int referenceYear) override {
        return "Resident is 59 years old and approaching pension age.";
    }
};

// =====================================================
// Concrete Strategy: IncompleteDataStatusStrategy
// =====================================================

class IncompleteDataStatusStrategy : public IResidentStatusStrategy {
public:
    ResidentStatus resolveStatus(Resident resident, int referenceYear) override {
        return ResidentStatus::DATA_TIDAK_LENGKAP;
    }

    string getReason(Resident resident, int referenceYear) override {
        return "Resident record is incomplete because required data is missing.";
    }
};

// =====================================================
// Concrete Strategy: InactiveResidentStatusStrategy
// =====================================================

class InactiveResidentStatusStrategy : public IResidentStatusStrategy {
public:
    ResidentStatus resolveStatus(Resident resident, int referenceYear) override {
        return ResidentStatus::TIDAK_LAYAK;
    }

    string getReason(Resident resident, int referenceYear) override {
        return "Resident has reached 60 years old and is no longer eligible.";
    }
};

// =====================================================
// Repository Interface
// Interface: IRepository
// =====================================================

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

// =====================================================
// Repository: ResidentRepository
// =====================================================

class ResidentRepository : public IRepository<Resident> {
private:
    vector<Resident> residents;

public:
    Resident findById(string id) override {
        for (Resident resident : residents) {
            if (resident.getId() == id) {
                return resident;
            }
        }

        return Resident();
    }

    vector<Resident> findMany(QueryFilter filter) override {
        vector<Resident> result;

        for (Resident resident : residents) {
            if (filter.keyword.empty() ||
                resident.getFullName().find(filter.keyword) != string::npos ||
                resident.getIcNumber().find(filter.keyword) != string::npos) {
                result.push_back(resident);
            }
        }

        return result;
    }

    Resident create(Resident resident) override {
        residents.push_back(resident);
        return resident;
    }

    Resident update(string id, Resident updatedResident) override {
        for (int i = 0; i < residents.size(); i++) {
            if (residents[i].getId() == id) {
                residents[i] = updatedResident;
                return residents[i];
            }
        }

        return Resident();
    }

    bool remove(string id) override {
        for (int i = 0; i < residents.size(); i++) {
            if (residents[i].getId() == id) {
                residents.erase(residents.begin() + i);
                return true;
            }
        }

        return false;
    }

    Resident findByIcNumber(string icNumber) {
        for (Resident resident : residents) {
            if (resident.getIcNumber() == icNumber) {
                return resident;
            }
        }

        return Resident();
    }

    vector<Resident> findAvailableResidents(AvailableResidentFilter filter) {
        vector<Resident> result;

        for (Resident resident : residents) {
            if (resident.getStatus() == ResidentStatus::AKTIF &&
                (filter.department.empty() || resident.getDepartment() == filter.department)) {
                result.push_back(resident);
            }
        }

        return result;
    }

    Resident findWithDetails(string id) {
        return findById(id);
    }

    Resident save(Resident resident) {
        Resident existingResident = findById(resident.getId());

        if (existingResident.getId().empty()) {
            return create(resident);
        }

        return update(resident.getId(), resident);
    }
};

// =====================================================
// Mapper Interface
// Interface: EntityMapper
// =====================================================

template <typename T, typename D>
class EntityMapper {
public:
    virtual D toDto(T entity) = 0;
    virtual vector<D> toDtoList(vector<T> entities) = 0;
    virtual ~EntityMapper() {}
};

// =====================================================
// Mapper: ResidentMapper
// =====================================================

class ResidentMapper : public EntityMapper<Resident, ResidentDto> {
public:
    ResidentDto toDto(Resident resident) override {
        return {
            resident.getId(),
            resident.getFullName(),
            resident.getIcNumber(),
            statusToString(resident.getStatus())
        };
    }

    vector<ResidentDto> toDtoList(vector<Resident> residents) override {
        vector<ResidentDto> result;

        for (Resident resident : residents) {
            result.push_back(toDto(resident));
        }

        return result;
    }

    ResidentDetailsDto toDetailsDto(Resident resident) {
        return {
            resident.getId(),
            resident.getFullName(),
            resident.getIcNumber(),
            resident.getPhone(),
            resident.getEmail(),
            resident.getPosition(),
            resident.getDepartment(),
            resident.getServiceLevel(),
            statusToString(resident.getStatus()),
            resident.getOutstandingBalanceValue()
        };
    }
};

// =====================================================
// Service Layer
// Class: ResidentService
// =====================================================

class ResidentService {
private:
    ResidentRepository& residentRepository;

public:
    ResidentService(ResidentRepository& repository)
        : residentRepository(repository) {
    }

    PaginatedResult<Resident> getResidents(ResidentFilter filter, int page, int limit) {
        QueryFilter queryFilter = { filter.keyword };
        vector<Resident> result = residentRepository.findMany(queryFilter);

        return { result, page, limit, static_cast<int>(result.size()) };
    }

    Resident createResident(ResidentCreateInput input, AdminProfile actor) {
        string newId = "RES_" + input.icNumber;
        Resident resident(newId, input);

        ResidentStatus status = resolveResidentStatus(resident, 2026);
        resident.changeStatus(status, "Status resolved during resident creation.");

        cout << "Resident created by admin: " << actor.username << endl;

        return residentRepository.create(resident);
    }

    Resident updateResident(string id, ResidentUpdateInput input, AdminProfile actor) {
        Resident resident = residentRepository.findById(id);

        if (resident.getId().empty()) {
            return Resident();
        }

        resident.updateDetails(input);

        ResidentStatus status = resolveResidentStatus(resident, 2026);
        resident.changeStatus(status, "Status resolved during resident update.");

        cout << "Resident updated by admin: " << actor.username << endl;

        return residentRepository.save(resident);
    }

    bool deleteResident(string id, AdminProfile actor) {
        cout << "Resident deleted by admin: " << actor.username << endl;
        return residentRepository.remove(id);
    }

    ResidentDetailsDto readResident(string id) {
        Resident resident = residentRepository.findWithDetails(id);
        ResidentMapper mapper;

        return mapper.toDetailsDto(resident);
    }

    vector<Resident> findAvailableResidents(AvailableResidentFilter filter) {
        return residentRepository.findAvailableResidents(filter);
    }

    ResidentStatus resolveResidentStatus(Resident resident, int referenceYear) {
        IResidentStatusStrategy* strategy;

        if (resident.isDataIncomplete()) {
            strategy = new IncompleteDataStatusStrategy();
        } else if (resident.calculateAge(referenceYear) >= 60) {
            strategy = new InactiveResidentStatusStrategy();
        } else if (resident.calculateAge(referenceYear) == 59) {
            strategy = new UpcomingPensionStatusStrategy();
        } else {
            strategy = new ActiveResidentStatusStrategy();
        }

        ResidentStatus status = strategy->resolveStatus(resident, referenceYear);
        cout << "Resolved reason: " << strategy->getReason(resident, referenceYear) << endl;

        delete strategy;
        return status;
    }
};

// =====================================================
// Structural Pattern: Facade
// Class: ResidentManagementFacade
// =====================================================
// This facade provides simple methods for resident management.
// Controllers can use this class without directly interacting with
// repository, mapper, and status strategy classes.

class ResidentManagementFacade {
private:
    ResidentService& residentService;

public:
    ResidentManagementFacade(ResidentService& service)
        : residentService(service) {
    }

    Resident createResident(ResidentCreateInput input, AdminProfile actor) {
        return residentService.createResident(input, actor);
    }

    Resident updateResident(string id, ResidentUpdateInput input, AdminProfile actor) {
        return residentService.updateResident(id, input, actor);
    }

    bool deleteResident(string id, AdminProfile actor) {
        return residentService.deleteResident(id, actor);
    }

    ResidentDetailsDto getResidentDetails(string id) {
        return residentService.readResident(id);
    }
};

// =====================================================
// Controller Layer
// Class: ResidentController
// =====================================================

class ResidentController {
private:
    ResidentManagementFacade& residentManagementFacade;
    ResidentService& residentService;
    ResidentMapper residentMapper;

public:
    ResidentController(ResidentManagementFacade& facade, ResidentService& service)
        : residentManagementFacade(facade),
          residentService(service) {
    }

    ApiResponse<PaginatedResult<ResidentDto>> getResidents(ResidentListRequest request) {
        PaginatedResult<Resident> residents =
            residentService.getResidents(request.filter, request.page, request.limit);

        PaginatedResult<ResidentDto> dtoResult = {
            residentMapper.toDtoList(residents.items),
            residents.page,
            residents.limit,
            residents.total
        };

        return { true, "Resident list retrieved successfully.", dtoResult };
    }

    ApiResponse<ResidentDto> postCreateResident(ResidentCreateRequest request) {
        Resident resident =
            residentManagementFacade.createResident(request.input, request.actor);

        return {
            true,
            "Resident created successfully.",
            residentMapper.toDto(resident)
        };
    }

    ApiResponse<ResidentDto> patchResident(ResidentUpdateRequest request) {
        Resident resident =
            residentManagementFacade.updateResident(request.id, request.input, request.actor);

        if (resident.getId().empty()) {
            return { false, "Resident not found.", ResidentDto() };
        }

        return {
            true,
            "Resident updated successfully.",
            residentMapper.toDto(resident)
        };
    }

    ApiResponse<ResidentDetailsDto> getResidentById(ResidentDetailRequest request) {
        ResidentDetailsDto details =
            residentManagementFacade.getResidentDetails(request.id);

        if (details.id.empty()) {
            return { false, "Resident not found.", ResidentDetailsDto() };
        }

        return {
            true,
            "Resident details retrieved successfully.",
            details
        };
    }

    ApiResponse<bool> deleteResident(ResidentDeleteRequest request) {
        bool deleted =
            residentManagementFacade.deleteResident(request.id, request.actor);

        return {
            deleted,
            deleted ? "Resident deleted successfully." : "Resident deletion failed.",
            deleted
        };
    }
};

// =====================================================
// Main Function
// =====================================================
// This main function demonstrates how the Resident Management
// subsystem follows the class diagram.

int main() {
    ResidentRepository residentRepository;
    ResidentService residentService(residentRepository);
    ResidentManagementFacade residentManagementFacade(residentService);
    ResidentController residentController(residentManagementFacade, residentService);

    AdminProfile admin = {
        "ADMIN_001",
        "adminUser"
    };

    ResidentCreateRequest createRequest = {
        {
            "Ali Bin Abu",
            "660101011111",
            "0123456789",
            "ali@johor.gov.my",
            "Clerk",
            "Finance Department",
            "N29"
        },
        admin
    };

    ApiResponse<ResidentDto> createResponse =
        residentController.postCreateResident(createRequest);

    cout << createResponse.message << endl;
    cout << "Resident Name: " << createResponse.data.fullName << endl;
    cout << "Resident Status: " << createResponse.data.status << endl;

    ResidentDetailRequest detailRequest = {
        createResponse.data.id
    };

    ApiResponse<ResidentDetailsDto> detailResponse =
        residentController.getResidentById(detailRequest);

    cout << "\nResident Details" << endl;
    cout << "Name: " << detailResponse.data.fullName << endl;
    cout << "IC Number: " << detailResponse.data.icNumber << endl;
    cout << "Department: " << detailResponse.data.department << endl;
    cout << "Outstanding Balance: RM "
         << detailResponse.data.outstandingBalance << endl;

    return 0;
}