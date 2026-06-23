#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>

using namespace std;

using Date = string;

// =====================================================
// Enumerations
// =====================================================

enum class EntityType {
    ADMIN_PROFILE,
    RESIDENT,
    QUARTER_CATEGORY,
    UNIT,
    UNIT_OCCUPANCY,
    MONTHLY_CHARGE,
    ADDITIONAL_CHARGE,
    REBATE,
    PAYMENT,
    TRANSACTION,
    ARREARS_SUMMARY
};

enum class AuditActionType {
    CREATE,
    UPDATE,
    DELETE_ACTION,
    VERIFY,
    LOGIN,
    LOGOUT,
    EXPORT,
    REVERSAL,
    ADJUSTMENT,
    IMPORT_EXTRACT
};

string entityTypeToString(EntityType type) {
    switch (type) {
        case EntityType::ADMIN_PROFILE:
            return "ADMIN_PROFILE";
        case EntityType::RESIDENT:
            return "RESIDENT";
        case EntityType::QUARTER_CATEGORY:
            return "QUARTER_CATEGORY";
        case EntityType::UNIT:
            return "UNIT";
        case EntityType::UNIT_OCCUPANCY:
            return "UNIT_OCCUPANCY";
        case EntityType::MONTHLY_CHARGE:
            return "MONTHLY_CHARGE";
        case EntityType::ADDITIONAL_CHARGE:
            return "ADDITIONAL_CHARGE";
        case EntityType::REBATE:
            return "REBATE";
        case EntityType::PAYMENT:
            return "PAYMENT";
        case EntityType::TRANSACTION:
            return "TRANSACTION";
        case EntityType::ARREARS_SUMMARY:
            return "ARREARS_SUMMARY";
        default:
            return "UNKNOWN";
    }
}

string auditActionTypeToString(AuditActionType type) {
    switch (type) {
        case AuditActionType::CREATE:
            return "CREATE";
        case AuditActionType::UPDATE:
            return "UPDATE";
        case AuditActionType::DELETE_ACTION:
            return "DELETE";
        case AuditActionType::VERIFY:
            return "VERIFY";
        case AuditActionType::LOGIN:
            return "LOGIN";
        case AuditActionType::LOGOUT:
            return "LOGOUT";
        case AuditActionType::EXPORT:
            return "EXPORT";
        case AuditActionType::REVERSAL:
            return "REVERSAL";
        case AuditActionType::ADJUSTMENT:
            return "ADJUSTMENT";
        case AuditActionType::IMPORT_EXTRACT:
            return "IMPORT_EXTRACT";
        default:
            return "UNKNOWN";
    }
}

// =====================================================
// Basic DTOs, Inputs and Request Models
// =====================================================

struct AdminProfile {
    string id;
    string fullName;
    string email;
};

template <typename T>
struct ApiResponse {
    bool success;
    string message;
    T data;
};

template <typename T>
struct PaginatedResult {
    vector<T> items;
    int page;
    int limit;
    int total;
};

struct QueryFilter {
    string keyword;
};

struct AuditLogFilters {
    string keyword;
    string moduleName;
    string userId;
    bool hasEntityType;
    EntityType entityType;
    bool hasActionType;
    AuditActionType actionType;
    Date startDate;
    Date endDate;
};

struct AuditLogFilterInput {
    string keyword;
    string moduleName;
    string userId;
    string entityTypeText;
    string actionTypeText;
    Date startDate;
    Date endDate;
};

struct AuditLogListRequest {
    int page;
    int limit;
    AuditLogFilterInput filters;
};

struct AuditLogDetailRequest {
    string id;
};

struct AuditLogExportRequest {
    AuditLogFilterInput filters;
};

struct CreateAuditLogInput {
    AdminProfile actor;
    string moduleName;
    AuditActionType actionType;
    EntityType entityType;
    string entityId;
    string target;
    string description;
};

struct AuditLogListItem {
    string id;
    string timestamp;
    string userName;
    string moduleName;
    string actionLabel;
    string target;
};

struct AuditLogDetailItem {
    string id;
    string timestamp;
    string userId;
    string userName;
    string moduleName;
    string actionLabel;
    string entityType;
    string target;
    string description;
};

struct AuditLogExportItem {
    string timestamp;
    string userName;
    string moduleName;
    string actionType;
    string entityType;
    string target;
    string description;
};

struct FileResponse {
    string fileName;
    string contentType;
    string content;
};

// =====================================================
// Domain Event Model
// =====================================================
// DomainEvent represents an important system activity that should be audited.

struct DomainEvent {
    AdminProfile actor;
    string moduleName;
    AuditActionType actionType;
    EntityType entityType;
    string entityId;
    string target;
    string description;
};

// =====================================================
// Domain Entity: AuditLog
// =====================================================

class AuditLog {
private:
    string id;
    Date timestamp;
    string userId;
    string userName;
    string moduleName;
    AuditActionType actionType;
    EntityType entityType;
    string entityId;
    string target;
    string description;

public:
    AuditLog() {
        id = "";
        timestamp = "";
        userId = "";
        userName = "";
        moduleName = "";
        actionType = AuditActionType::CREATE;
        entityType = EntityType::RESIDENT;
        entityId = "";
        target = "";
        description = "";
    }

    AuditLog(
        string id,
        Date timestamp,
        string userId,
        string userName,
        string moduleName,
        AuditActionType actionType,
        EntityType entityType,
        string entityId,
        string target,
        string description
    ) {
        this->id = id;
        this->timestamp = timestamp;
        this->userId = userId;
        this->userName = userName;
        this->moduleName = moduleName;
        this->actionType = actionType;
        this->entityType = entityType;
        this->entityId = entityId;
        this->target = target;
        this->description = description;
    }

    string formatActionLabel() const {
        return auditActionTypeToString(actionType) + " " + entityTypeToString(entityType);
    }

    string formatTarget() const {
        if (!target.empty()) {
            return target;
        }

        return entityTypeToString(entityType) + " #" + entityId;
    }

    AuditLogListItem toListItem() const {
        return {
            id,
            timestamp,
            userName,
            moduleName,
            formatActionLabel(),
            formatTarget()
        };
    }

    AuditLogDetailItem toDetailItem() const {
        return {
            id,
            timestamp,
            userId,
            userName,
            moduleName,
            formatActionLabel(),
            entityTypeToString(entityType),
            formatTarget(),
            description
        };
    }

    AuditLogExportItem toExportItem() const {
        return {
            timestamp,
            userName,
            moduleName,
            auditActionTypeToString(actionType),
            entityTypeToString(entityType),
            formatTarget(),
            description
        };
    }

    string getId() const {
        return id;
    }

    Date getTimestamp() const {
        return timestamp;
    }

    string getUserId() const {
        return userId;
    }

    string getUserName() const {
        return userName;
    }

    string getModuleName() const {
        return moduleName;
    }

    AuditActionType getActionType() const {
        return actionType;
    }

    EntityType getEntityType() const {
        return entityType;
    }

    string getEntityId() const {
        return entityId;
    }

    string getTarget() const {
        return target;
    }

    string getDescription() const {
        return description;
    }
};

// =====================================================
// Creational Pattern: Builder
// Class: AuditLogBuilder
// =====================================================
// The builder creates an AuditLog step by step so audit records remain clear.

class AuditLogBuilder {
private:
    AdminProfile actor;
    string moduleName;
    AuditActionType actionType;
    EntityType entityType;
    string entityId;
    string target;
    string description;

public:
    AuditLogBuilder() {
        actor = { "SYSTEM", "System", "system@local" };
        moduleName = "System";
        actionType = AuditActionType::CREATE;
        entityType = EntityType::RESIDENT;
        entityId = "";
        target = "";
        description = "";
    }

    AuditLogBuilder& forActor(AdminProfile actor) {
        this->actor = actor;
        return *this;
    }

    AuditLogBuilder& forModule(string moduleName) {
        this->moduleName = moduleName;
        return *this;
    }

    AuditLogBuilder& withAction(AuditActionType actionType) {
        this->actionType = actionType;
        return *this;
    }

    AuditLogBuilder& withTarget(
        EntityType entityType,
        string entityId,
        string target
    ) {
        this->entityType = entityType;
        this->entityId = entityId;
        this->target = target;
        return *this;
    }

    AuditLogBuilder& withDescription(string description) {
        this->description = description;
        return *this;
    }

    AuditLog build() {
        string generatedId = "AUDIT_" + actor.id + "_" + entityId;
        Date currentTimestamp = "2026-06-23 15:00:00";

        return AuditLog(
            generatedId,
            currentTimestamp,
            actor.id,
            actor.fullName,
            moduleName,
            actionType,
            entityType,
            entityId,
            target,
            description
        );
    }
};

// =====================================================
// Repository Interface
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
// Repository: AuditLogRepository
// =====================================================

class AuditLogRepository : public IRepository<AuditLog> {
private:
    vector<AuditLog> logs;

    bool matchesFilters(AuditLog log, AuditLogFilters filters) {
        bool matchesKeyword =
            filters.keyword.empty() ||
            log.getUserName().find(filters.keyword) != string::npos ||
            log.getModuleName().find(filters.keyword) != string::npos ||
            log.formatTarget().find(filters.keyword) != string::npos ||
            log.getDescription().find(filters.keyword) != string::npos;

        bool matchesModule =
            filters.moduleName.empty() ||
            log.getModuleName() == filters.moduleName;

        bool matchesUser =
            filters.userId.empty() ||
            log.getUserId() == filters.userId;

        bool matchesEntity =
            !filters.hasEntityType ||
            log.getEntityType() == filters.entityType;

        bool matchesAction =
            !filters.hasActionType ||
            log.getActionType() == filters.actionType;

        bool matchesStartDate =
            filters.startDate.empty() ||
            log.getTimestamp() >= filters.startDate;

        bool matchesEndDate =
            filters.endDate.empty() ||
            log.getTimestamp() <= filters.endDate;

        return matchesKeyword &&
               matchesModule &&
               matchesUser &&
               matchesEntity &&
               matchesAction &&
               matchesStartDate &&
               matchesEndDate;
    }

public:
    AuditLogRepository() {
        AdminProfile systemActor = {
            "ADMIN_001",
            "Galen GUI",
            "admin@johor.gov.my"
        };

        logs.push_back(
            AuditLogBuilder()
                .forActor(systemActor)
                .forModule("Authentication")
                .withAction(AuditActionType::LOGIN)
                .withTarget(EntityType::ADMIN_PROFILE, "ADMIN_001", "Galen GUI")
                .withDescription("Administrator logged into the system.")
                .build()
        );
    }

    AuditLog findById(string id) override {
        for (AuditLog log : logs) {
            if (log.getId() == id) {
                return log;
            }
        }

        return AuditLog();
    }

    vector<AuditLog> findMany(QueryFilter filter) override {
        vector<AuditLog> result;

        for (AuditLog log : logs) {
            if (filter.keyword.empty() ||
                log.getUserName().find(filter.keyword) != string::npos ||
                log.getModuleName().find(filter.keyword) != string::npos ||
                log.formatTarget().find(filter.keyword) != string::npos) {
                result.push_back(log);
            }
        }

        return result;
    }

    AuditLog create(AuditLog input) override {
        logs.push_back(input);
        return input;
    }

    AuditLog update(string id, AuditLog input) override {
        for (int i = 0; i < static_cast<int>(logs.size()); i++) {
            if (logs[i].getId() == id) {
                logs[i] = input;
                return logs[i];
            }
        }

        return AuditLog();
    }

    bool remove(string id) override {
        for (int i = 0; i < static_cast<int>(logs.size()); i++) {
            if (logs[i].getId() == id) {
                logs.erase(logs.begin() + i);
                return true;
            }
        }

        return false;
    }

    PaginatedResult<AuditLog> findPage(
        int page,
        int limit,
        AuditLogFilters filters
    ) {
        vector<AuditLog> filteredLogs;

        for (AuditLog log : logs) {
            if (matchesFilters(log, filters)) {
                filteredLogs.push_back(log);
            }
        }

        return {
            filteredLogs,
            page,
            limit,
            static_cast<int>(filteredLogs.size())
        };
    }

    AuditLog findDetail(string id) {
        return findById(id);
    }

    vector<AuditLogExportItem> findExportRows(AuditLogFilters filters) {
        vector<AuditLogExportItem> rows;

        for (AuditLog log : logs) {
            if (matchesFilters(log, filters)) {
                rows.push_back(log.toExportItem());
            }
        }

        return rows;
    }

    AuditLog save(AuditLog log) {
        AuditLog existing = findById(log.getId());

        if (existing.getId().empty()) {
            return create(log);
        }

        return update(log.getId(), log);
    }
};

// =====================================================
// Behavioral Pattern: Observer
// Interface: DomainEventObserver
// =====================================================

class DomainEventObserver {
public:
    virtual void onDomainEvent(DomainEvent event) = 0;
    virtual ~DomainEventObserver() {}
};

// =====================================================
// Behavioral Pattern: Observer Publisher
// Class: DomainEventPublisher
// =====================================================

class DomainEventPublisher {
private:
    vector<DomainEventObserver*> observers;

public:
    void subscribe(DomainEventObserver* observer) {
        observers.push_back(observer);
    }

    void unsubscribe(DomainEventObserver* observer) {
        observers.erase(
            remove(observers.begin(), observers.end(), observer),
            observers.end()
        );
    }

    void publish(DomainEvent event) {
        for (DomainEventObserver* observer : observers) {
            observer->onDomainEvent(event);
        }
    }
};

// =====================================================
// Service Layer
// Class: AuditLogService
// =====================================================

class AuditLogService {
private:
    AuditLogRepository& auditLogRepository;

    EntityType parseEntityType(string value, bool& hasEntityType) {
        hasEntityType = true;

        if (value == "ADMIN_PROFILE") return EntityType::ADMIN_PROFILE;
        if (value == "RESIDENT") return EntityType::RESIDENT;
        if (value == "QUARTER_CATEGORY") return EntityType::QUARTER_CATEGORY;
        if (value == "UNIT") return EntityType::UNIT;
        if (value == "UNIT_OCCUPANCY") return EntityType::UNIT_OCCUPANCY;
        if (value == "MONTHLY_CHARGE") return EntityType::MONTHLY_CHARGE;
        if (value == "ADDITIONAL_CHARGE") return EntityType::ADDITIONAL_CHARGE;
        if (value == "REBATE") return EntityType::REBATE;
        if (value == "PAYMENT") return EntityType::PAYMENT;
        if (value == "TRANSACTION") return EntityType::TRANSACTION;
        if (value == "ARREARS_SUMMARY") return EntityType::ARREARS_SUMMARY;

        hasEntityType = false;
        return EntityType::RESIDENT;
    }

    AuditActionType parseActionType(string value, bool& hasActionType) {
        hasActionType = true;

        if (value == "CREATE") return AuditActionType::CREATE;
        if (value == "UPDATE") return AuditActionType::UPDATE;
        if (value == "DELETE") return AuditActionType::DELETE_ACTION;
        if (value == "VERIFY") return AuditActionType::VERIFY;
        if (value == "LOGIN") return AuditActionType::LOGIN;
        if (value == "LOGOUT") return AuditActionType::LOGOUT;
        if (value == "EXPORT") return AuditActionType::EXPORT;
        if (value == "REVERSAL") return AuditActionType::REVERSAL;
        if (value == "ADJUSTMENT") return AuditActionType::ADJUSTMENT;
        if (value == "IMPORT_EXTRACT") return AuditActionType::IMPORT_EXTRACT;

        hasActionType = false;
        return AuditActionType::CREATE;
    }

public:
    AuditLogService(AuditLogRepository& repository)
        : auditLogRepository(repository) {
    }

    AuditLog createAuditLog(CreateAuditLogInput input) {
        AuditLog log = AuditLogBuilder()
            .forActor(input.actor)
            .forModule(input.moduleName)
            .withAction(input.actionType)
            .withTarget(input.entityType, input.entityId, input.target)
            .withDescription(input.description)
            .build();

        return auditLogRepository.save(log);
    }

    PaginatedResult<AuditLog> getAuditLogPage(
        int page,
        int limit,
        AuditLogFilters filters
    ) {
        return auditLogRepository.findPage(page, limit, filters);
    }

    AuditLog getAuditLogDetail(string id) {
        return auditLogRepository.findDetail(id);
    }

    vector<AuditLogExportItem> getAuditLogExportRows(
        AuditLogFilters filters
    ) {
        return auditLogRepository.findExportRows(filters);
    }

    AuditLogFilters parseAuditLogFilters(AuditLogFilterInput input) {
        bool hasEntityType = false;
        bool hasActionType = false;

        EntityType entityType =
            parseEntityType(input.entityTypeText, hasEntityType);

        AuditActionType actionType =
            parseActionType(input.actionTypeText, hasActionType);

        return {
            input.keyword,
            input.moduleName,
            input.userId,
            hasEntityType,
            entityType,
            hasActionType,
            actionType,
            input.startDate,
            input.endDate
        };
    }

    string buildAuditTarget(EntityType entityType, string entityId) {
        return entityTypeToString(entityType) + " #" + entityId;
    }
};

// =====================================================
// Behavioral Pattern: Observer
// Class: AuditLogObserver
// =====================================================

class AuditLogObserver : public DomainEventObserver {
private:
    AuditLogService& auditLogService;

public:
    AuditLogObserver(AuditLogService& service)
        : auditLogService(service) {
    }

    AuditLog buildAuditLog(DomainEvent event) {
        return AuditLogBuilder()
            .forActor(event.actor)
            .forModule(event.moduleName)
            .withAction(event.actionType)
            .withTarget(event.entityType, event.entityId, event.target)
            .withDescription(event.description)
            .build();
    }

    void onDomainEvent(DomainEvent event) override {
        AuditLog log = buildAuditLog(event);

        auditLogService.createAuditLog({
            event.actor,
            event.moduleName,
            event.actionType,
            event.entityType,
            event.entityId,
            event.target,
            event.description
        });

        cout << "Audit log observer recorded event: "
             << log.formatActionLabel()
             << " -> "
             << log.formatTarget()
             << endl;
    }
};

// =====================================================
// Controller Layer
// Class: AuditLogController
// =====================================================

class AuditLogController {
private:
    AuditLogService& auditLogService;

public:
    AuditLogController(AuditLogService& service)
        : auditLogService(service) {
    }

    ApiResponse<PaginatedResult<AuditLogListItem>> getAuditLogs(
        AuditLogListRequest request
    ) {
        AuditLogFilters filters =
            auditLogService.parseAuditLogFilters(request.filters);

        PaginatedResult<AuditLog> page =
            auditLogService.getAuditLogPage(
                request.page,
                request.limit,
                filters
            );

        vector<AuditLogListItem> listItems;

        for (AuditLog log : page.items) {
            listItems.push_back(log.toListItem());
        }

        PaginatedResult<AuditLogListItem> result = {
            listItems,
            page.page,
            page.limit,
            page.total
        };

        return {
            true,
            "Audit logs retrieved successfully.",
            result
        };
    }

    ApiResponse<AuditLogDetailItem> getAuditLogDetail(
        AuditLogDetailRequest request
    ) {
        AuditLog log =
            auditLogService.getAuditLogDetail(request.id);

        if (log.getId().empty()) {
            return {
                false,
                "Audit log detail not found.",
                AuditLogDetailItem()
            };
        }

        return {
            true,
            "Audit log detail retrieved successfully.",
            log.toDetailItem()
        };
    }

    ApiResponse<FileResponse> exportAuditLogs(
        AuditLogExportRequest request
    ) {
        AuditLogFilters filters =
            auditLogService.parseAuditLogFilters(request.filters);

        vector<AuditLogExportItem> rows =
            auditLogService.getAuditLogExportRows(filters);

        string csv =
            "Timestamp,User,Module,Action,Entity,Target,Description\n";

        for (AuditLogExportItem row : rows) {
            csv += row.timestamp + "," +
                   row.userName + "," +
                   row.moduleName + "," +
                   row.actionType + "," +
                   row.entityType + "," +
                   row.target + "," +
                   row.description + "\n";
        }

        FileResponse file = {
            "audit_logs.csv",
            "text/csv",
            csv
        };

        return {
            true,
            "Audit logs exported successfully.",
            file
        };
    }
};

// =====================================================
// Main Function
// =====================================================

int main() {
    AuditLogRepository auditLogRepository;
    AuditLogService auditLogService(auditLogRepository);
    AuditLogObserver auditLogObserver(auditLogService);
    DomainEventPublisher domainEventPublisher;

    domainEventPublisher.subscribe(&auditLogObserver);

    AuditLogController auditLogController(auditLogService);

    AdminProfile actor = {
        "ADMIN_001",
        "Galen GUI",
        "admin@johor.gov.my"
    };

    DomainEvent residentCreatedEvent = {
        actor,
        "Resident Management",
        AuditActionType::CREATE,
        EntityType::RESIDENT,
        "RES_001",
        "Ali Bin Abu",
        "A new resident record was created."
    };

    DomainEvent paymentVerifiedEvent = {
        actor,
        "Payment Review",
        AuditActionType::VERIFY,
        EntityType::PAYMENT,
        "PAY_001",
        "Receipt RCP-001",
        "A payment draft was verified and saved."
    };

    domainEventPublisher.publish(residentCreatedEvent);
    domainEventPublisher.publish(paymentVerifiedEvent);

    AuditLogListRequest listRequest = {
        1,
        10,
        {
            "",
            "",
            "",
            "",
            "",
            "",
            ""
        }
    };

    ApiResponse<PaginatedResult<AuditLogListItem>> listResponse =
        auditLogController.getAuditLogs(listRequest);

    cout << "\n===== Audit Log List =====" << endl;
    cout << listResponse.message << endl;

    for (AuditLogListItem item : listResponse.data.items) {
        cout << item.id
             << " | " << item.timestamp
             << " | " << item.userName
             << " | " << item.moduleName
             << " | " << item.actionLabel
             << " | " << item.target
             << endl;
    }

    if (!listResponse.data.items.empty()) {
        AuditLogDetailRequest detailRequest = {
            listResponse.data.items[0].id
        };

        ApiResponse<AuditLogDetailItem> detailResponse =
            auditLogController.getAuditLogDetail(detailRequest);

        cout << "\n===== Audit Log Detail =====" << endl;
        cout << detailResponse.message << endl;

        if (detailResponse.success) {
            cout << "ID: " << detailResponse.data.id << endl;
            cout << "User: " << detailResponse.data.userName << endl;
            cout << "Module: " << detailResponse.data.moduleName << endl;
            cout << "Action: " << detailResponse.data.actionLabel << endl;
            cout << "Target: " << detailResponse.data.target << endl;
            cout << "Description: " << detailResponse.data.description << endl;
        }
    }

    AuditLogExportRequest exportRequest = {
        {
            "",
            "",
            "",
            "",
            "",
            "",
            ""
        }
    };

    ApiResponse<FileResponse> exportResponse =
        auditLogController.exportAuditLogs(exportRequest);

    cout << "\n===== Audit Log Export =====" << endl;
    cout << exportResponse.message << endl;
    cout << "File Name: " << exportResponse.data.fileName << endl;
    cout << "Content Type: " << exportResponse.data.contentType << endl;
    cout << exportResponse.data.content << endl;

    domainEventPublisher.unsubscribe(&auditLogObserver);

    return 0;
}
