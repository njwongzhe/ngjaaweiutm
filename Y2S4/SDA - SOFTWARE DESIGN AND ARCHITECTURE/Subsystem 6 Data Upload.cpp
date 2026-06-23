#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>

using namespace std;

using Date = string;
using Decimal = double;

// =====================================================
// Enumerations
// =====================================================

enum class DocumentCategory {
    BAYARAN,
    TUNGGAKAN,
    PENGHUNI,
    KUARTERS
};

enum class UnitStatus {
    OCCUPIED,
    VACANT
};

enum class ParsingMode {
    PDF,
    EXCEL
};

string documentCategoryToString(DocumentCategory category) {
    switch (category) {
        case DocumentCategory::BAYARAN:
            return "BAYARAN";
        case DocumentCategory::TUNGGAKAN:
            return "TUNGGAKAN";
        case DocumentCategory::PENGHUNI:
            return "PENGHUNI";
        case DocumentCategory::KUARTERS:
            return "KUARTERS";
        default:
            return "UNKNOWN";
    }
}

string unitStatusToString(UnitStatus status) {
    return status == UnitStatus::OCCUPIED ? "OCCUPIED" : "VACANT";
}

// =====================================================
// Basic DTOs and Request Models
// =====================================================

struct AdminProfile {
    string id;
    string username;
};

struct ValidationResult {
    bool valid;
    string message;
};

struct VerificationResult {
    bool success;
    string message;
    int verifiedCount;
};

struct ProcessingDraft {
    string documentId;
    string category;
    int draftCount;
    string status;
};

template <typename T>
struct ApiResponse {
    bool success;
    string message;
    T data;
};

struct UploadedDocumentDto {
    string id;
    string fileName;
    string fileType;
    int fileSize;
    string category;
    string remarks;
    int draftCount;
};

struct UploadedDocumentListRequest {
    string category;
};

struct UploadedDocumentCreateRequest {
    string fileName;
    string fileType;
    int fileSize;
    DocumentCategory category;
    string remarks;
    ParsingMode parsingMode;
    AdminProfile actor;
};

struct DraftUpdateRequest {
    string documentId;
    string draftId;
    string newRemark;
    AdminProfile actor;
};

struct DocumentVerifyRequest {
    string documentId;
    DocumentCategory category;
    vector<string> selectedKeys;
    AdminProfile actor;
};

struct UploadedDocumentDeleteRequest {
    string documentId;
    AdminProfile actor;
};

struct CreateUploadedDocumentPayload {
    string fileName;
    string fileType;
    int fileSize;
    string remarks;
    ParsingMode parsingMode;
};

struct DraftUpdateInput {
    string newRemark;
};

struct QueryFilter {
    string keyword;
};

struct ExtractResult {
    vector<string> rows;
};

struct UploadedFile {
    string fileName;
    string fileType;
    int fileSize;
};

struct VerificationContext {
    string documentId;
    AdminProfile actor;
};

// =====================================================
// Input Models
// =====================================================

struct QuarterCategoryCreateInput {
    string categoryName;
    string address;
    Decimal rentalPrice;
    Decimal maintenancePrice;
    Decimal penaltyPrice;
};

struct UnitCreateInput {
    string unitCode;
    UnitStatus status;
    string categoryId;
};

struct ResidentCreateInput {
    string fullName;
    string icNumber;
    string unitCode;
};

struct PaymentCreateInput {
    string residentId;
    Date paymentDate;
    string receiptNo;
    Decimal amount;
};

struct ArrearsUpdateInput {
    string residentId;
    Decimal totalArrearsAmount;
};

// =====================================================
// Domain Entities Used by Draft Verification
// =====================================================

class QuarterCategory {
private:
    string id;
    string categoryName;
    string address;
    Decimal rentalPrice;
    Decimal maintenancePrice;
    Decimal penaltyPrice;

public:
    QuarterCategory() {
        id = "";
        categoryName = "";
        address = "";
        rentalPrice = 0.0;
        maintenancePrice = 0.0;
        penaltyPrice = 0.0;
    }

    QuarterCategory(
        string id,
        string categoryName,
        string address,
        Decimal rentalPrice,
        Decimal maintenancePrice,
        Decimal penaltyPrice
    ) {
        this->id = id;
        this->categoryName = categoryName;
        this->address = address;
        this->rentalPrice = rentalPrice;
        this->maintenancePrice = maintenancePrice;
        this->penaltyPrice = penaltyPrice;
    }

    string getId() const {
        return id;
    }

    string getCategoryName() const {
        return categoryName;
    }
};

class Unit {
private:
    string id;
    string unitCode;
    UnitStatus status;
    string categoryId;

public:
    Unit() {
        id = "";
        unitCode = "";
        status = UnitStatus::VACANT;
        categoryId = "";
    }

    Unit(string id, string unitCode, UnitStatus status, string categoryId) {
        this->id = id;
        this->unitCode = unitCode;
        this->status = status;
        this->categoryId = categoryId;
    }

    string getId() const {
        return id;
    }

    string getUnitCode() const {
        return unitCode;
    }
};

class Resident {
private:
    string id;
    string fullName;
    string icNumber;
    string unitCode;

public:
    Resident() {
        id = "";
        fullName = "";
        icNumber = "";
        unitCode = "";
    }

    Resident(string id, string fullName, string icNumber, string unitCode) {
        this->id = id;
        this->fullName = fullName;
        this->icNumber = icNumber;
        this->unitCode = unitCode;
    }

    string getId() const {
        return id;
    }

    string getIcNumber() const {
        return icNumber;
    }

    string getFullName() const {
        return fullName;
    }
};

class Payment {
private:
    string id;
    string residentId;
    Date paymentDate;
    string receiptNo;
    Decimal amount;

public:
    Payment() {
        id = "";
        residentId = "";
        paymentDate = "";
        receiptNo = "";
        amount = 0.0;
    }

    Payment(
        string id,
        string residentId,
        Date paymentDate,
        string receiptNo,
        Decimal amount
    ) {
        this->id = id;
        this->residentId = residentId;
        this->paymentDate = paymentDate;
        this->receiptNo = receiptNo;
        this->amount = amount;
    }

    string getId() const {
        return id;
    }

    string getReceiptNo() const {
        return receiptNo;
    }
};

class ArrearsSummary {
private:
    string id;
    string residentId;
    Decimal totalArrearsAmount;

public:
    ArrearsSummary() {
        id = "";
        residentId = "";
        totalArrearsAmount = 0.0;
    }

    ArrearsSummary(string id, string residentId, Decimal totalArrearsAmount) {
        this->id = id;
        this->residentId = residentId;
        this->totalArrearsAmount = totalArrearsAmount;
    }

    string getId() const {
        return id;
    }

    Decimal getTotalArrearsAmount() const {
        return totalArrearsAmount;
    }
};

// =====================================================
// Abstract Domain Entity: DocumentDraft
// =====================================================

class DocumentDraft {
protected:
    string id;
    string uploadedDocumentId;
    string remark;
    bool pendingVerification;

public:
    DocumentDraft(string id, string uploadedDocumentId) {
        this->id = id;
        this->uploadedDocumentId = uploadedDocumentId;
        this->remark = "";
        this->pendingVerification = true;
    }

    virtual void update(DraftUpdateInput input) {
        remark = input.newRemark;
    }

    virtual bool removeDraft() {
        pendingVerification = false;
        return true;
    }

    virtual ValidationResult validate() = 0;

    bool isPendingVerification() const {
        return pendingVerification;
    }

    string getSourceDocumentId() const {
        return uploadedDocumentId;
    }

    string getId() const {
        return id;
    }

    virtual string getDraftType() const = 0;

    virtual ~DocumentDraft() {}
};

// =====================================================
// Concrete Draft: QuarterCategoryDraft
// =====================================================

class QuarterCategoryDraft : public DocumentDraft {
private:
    string categoryName;
    string address;
    Decimal rentalPrice;
    Decimal maintenancePrice;
    Decimal penaltyPrice;

public:
    QuarterCategoryDraft(
        string id,
        string uploadedDocumentId,
        string categoryName,
        string address,
        Decimal rentalPrice,
        Decimal maintenancePrice,
        Decimal penaltyPrice
    ) : DocumentDraft(id, uploadedDocumentId) {
        this->categoryName = categoryName;
        this->address = address;
        this->rentalPrice = rentalPrice;
        this->maintenancePrice = maintenancePrice;
        this->penaltyPrice = penaltyPrice;
    }

    QuarterCategoryCreateInput toCategoryInput() {
        return {
            categoryName,
            address,
            rentalPrice,
            maintenancePrice,
            penaltyPrice
        };
    }

    QuarterCategory resolveExistingCategory() {
        return QuarterCategory("", "", "", 0.0, 0.0, 0.0);
    }

    QuarterCategory verify(VerificationContext context) {
        pendingVerification = false;

        cout << "Quarter category draft verified by "
             << context.actor.username << endl;

        return QuarterCategory(
            "QCAT_" + id,
            categoryName,
            address,
            rentalPrice,
            maintenancePrice,
            penaltyPrice
        );
    }

    ValidationResult validate() override {
        if (categoryName.empty() || address.empty()) {
            return { false, "Quarter category name and address are required." };
        }

        return { true, "Quarter category draft is valid." };
    }

    string getDraftType() const override {
        return "QuarterCategoryDraft";
    }
};

// =====================================================
// Concrete Draft: UnitDraft
// =====================================================

class UnitDraft : public DocumentDraft {
private:
    string unitCode;
    UnitStatus status;
    string categoryDraftId;

public:
    UnitDraft(
        string id,
        string uploadedDocumentId,
        string unitCode,
        UnitStatus status,
        string categoryDraftId
    ) : DocumentDraft(id, uploadedDocumentId) {
        this->unitCode = unitCode;
        this->status = status;
        this->categoryDraftId = categoryDraftId;
    }

    UnitCreateInput toUnitInput() {
        return {
            unitCode,
            status,
            categoryDraftId
        };
    }

    Unit resolveExistingUnit() {
        return Unit("", "", UnitStatus::VACANT, "");
    }

    Unit verify(VerificationContext context) {
        pendingVerification = false;

        cout << "Unit draft verified by "
             << context.actor.username << endl;

        return Unit(
            "UNIT_" + id,
            unitCode,
            status,
            categoryDraftId
        );
    }

    ValidationResult validate() override {
        if (unitCode.empty()) {
            return { false, "Unit code is required." };
        }

        return { true, "Unit draft is valid." };
    }

    string getDraftType() const override {
        return "UnitDraft";
    }
};

// =====================================================
// Concrete Draft: ResidentDraft
// =====================================================

class ResidentDraft : public DocumentDraft {
private:
    string fullName;
    string icNumber;
    string unitCode;

public:
    ResidentDraft(
        string id,
        string uploadedDocumentId,
        string fullName,
        string icNumber,
        string unitCode
    ) : DocumentDraft(id, uploadedDocumentId) {
        this->fullName = fullName;
        this->icNumber = icNumber;
        this->unitCode = unitCode;
    }

    ResidentCreateInput toResidentInput() {
        return {
            fullName,
            icNumber,
            unitCode
        };
    }

    Resident resolveExistingResident() {
        return Resident("", "", "", "");
    }

    Resident verify(VerificationContext context) {
        pendingVerification = false;

        cout << "Resident draft verified by "
             << context.actor.username << endl;

        return Resident(
            "RES_" + id,
            fullName,
            icNumber,
            unitCode
        );
    }

    ValidationResult validate() override {
        if (fullName.empty() || icNumber.empty()) {
            return { false, "Resident full name and IC number are required." };
        }

        return { true, "Resident draft is valid." };
    }

    string getDraftType() const override {
        return "ResidentDraft";
    }
};

// =====================================================
// Concrete Draft: PaymentDraft
// =====================================================

class PaymentDraft : public DocumentDraft {
private:
    string residentName;
    string residentIcNumber;
    Date paymentDate;
    string receiptNo;
    Decimal amount;

public:
    PaymentDraft(
        string id,
        string uploadedDocumentId,
        string residentName,
        string residentIcNumber,
        Date paymentDate,
        string receiptNo,
        Decimal amount
    ) : DocumentDraft(id, uploadedDocumentId) {
        this->residentName = residentName;
        this->residentIcNumber = residentIcNumber;
        this->paymentDate = paymentDate;
        this->receiptNo = receiptNo;
        this->amount = amount;
    }

    PaymentCreateInput toPaymentInput(string resolvedResidentId) {
        return {
            resolvedResidentId,
            paymentDate,
            receiptNo,
            amount
        };
    }

    Resident resolveResident() {
        return Resident(
            "RES_" + residentIcNumber,
            residentName,
            residentIcNumber,
            ""
        );
    }

    bool isDuplicate() {
        return false;
    }

    Payment verify(VerificationContext context) {
        pendingVerification = false;

        Resident resident = resolveResident();

        cout << "Payment draft verified by "
             << context.actor.username << endl;

        return Payment(
            "PAY_" + id,
            resident.getId(),
            paymentDate,
            receiptNo,
            amount
        );
    }

    ValidationResult validate() override {
        if (residentIcNumber.empty() || receiptNo.empty() || amount <= 0) {
            return { false, "Payment draft requires resident IC, receipt number and valid amount." };
        }

        return { true, "Payment draft is valid." };
    }

    string getDraftType() const override {
        return "PaymentDraft";
    }
};

// =====================================================
// Concrete Draft: ArrearsSummaryDraft
// =====================================================

class ArrearsSummaryDraft : public DocumentDraft {
private:
    string residentName;
    string residentIcNumber;
    Decimal totalArrearsAmount;

public:
    ArrearsSummaryDraft(
        string id,
        string uploadedDocumentId,
        string residentName,
        string residentIcNumber,
        Decimal totalArrearsAmount
    ) : DocumentDraft(id, uploadedDocumentId) {
        this->residentName = residentName;
        this->residentIcNumber = residentIcNumber;
        this->totalArrearsAmount = totalArrearsAmount;
    }

    ArrearsUpdateInput toArrearsInput(string resolvedResidentId) {
        return {
            resolvedResidentId,
            totalArrearsAmount
        };
    }

    Resident resolveResident() {
        return Resident(
            "RES_" + residentIcNumber,
            residentName,
            residentIcNumber,
            ""
        );
    }

    ArrearsSummary verify(VerificationContext context) {
        pendingVerification = false;

        Resident resident = resolveResident();

        cout << "Arrears summary draft verified by "
             << context.actor.username << endl;

        return ArrearsSummary(
            "ARR_" + id,
            resident.getId(),
            totalArrearsAmount
        );
    }

    ValidationResult validate() override {
        if (residentIcNumber.empty() || totalArrearsAmount < 0) {
            return { false, "Arrears draft requires resident IC and non-negative arrears amount." };
        }

        return { true, "Arrears summary draft is valid." };
    }

    string getDraftType() const override {
        return "ArrearsSummaryDraft";
    }
};

// =====================================================
// Domain Entity: UploadedDocument
// =====================================================

class UploadedDocument {
private:
    string id;
    string fileName;
    string fileType;
    int fileSize;
    DocumentCategory category;
    string uploadedById;
    string remarks;
    vector<shared_ptr<DocumentDraft>> drafts;

public:
    UploadedDocument() {
        id = "";
        fileName = "";
        fileType = "";
        fileSize = 0;
        category = DocumentCategory::PENGHUNI;
        uploadedById = "";
        remarks = "";
    }

    UploadedDocument(
        string id,
        string fileName,
        string fileType,
        int fileSize,
        DocumentCategory category,
        string uploadedById,
        string remarks
    ) {
        this->id = id;
        this->fileName = fileName;
        this->fileType = fileType;
        this->fileSize = fileSize;
        this->category = category;
        this->uploadedById = uploadedById;
        this->remarks = remarks;
    }

    UploadedDocument attachDrafts(vector<shared_ptr<DocumentDraft>> drafts) {
        this->drafts = drafts;
        return *this;
    }

    UploadedDocument updateRemark(string remarks) {
        this->remarks = remarks;
        return *this;
    }

    UploadedDocument markPendingReview() {
        remarks = "Pending human verification.";
        return *this;
    }

    UploadedDocument markVerified(string description) {
        remarks = "Verified: " + description;
        return *this;
    }

    int getDraftCount() const {
        return static_cast<int>(drafts.size());
    }

    bool belongsToCategory(DocumentCategory category) const {
        return this->category == category;
    }

    bool hasPendingDrafts() const {
        for (shared_ptr<DocumentDraft> draft : drafts) {
            if (draft->isPendingVerification()) {
                return true;
            }
        }

        return false;
    }

    string getId() const {
        return id;
    }

    string getFileName() const {
        return fileName;
    }

    string getFileType() const {
        return fileType;
    }

    int getFileSize() const {
        return fileSize;
    }

    DocumentCategory getCategory() const {
        return category;
    }

    string getRemarks() const {
        return remarks;
    }

    vector<shared_ptr<DocumentDraft>> getDrafts() const {
        return drafts;
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
// Repository: DocumentRepository
// =====================================================

class DocumentRepository : public IRepository<UploadedDocument> {
private:
    vector<UploadedDocument> documents;

public:
    UploadedDocument findById(string id) override {
        for (UploadedDocument document : documents) {
            if (document.getId() == id) {
                return document;
            }
        }

        return UploadedDocument();
    }

    vector<UploadedDocument> findMany(QueryFilter filter) override {
        vector<UploadedDocument> result;

        for (UploadedDocument document : documents) {
            if (filter.keyword.empty() ||
                document.getFileName().find(filter.keyword) != string::npos) {
                result.push_back(document);
            }
        }

        return result;
    }

    UploadedDocument create(UploadedDocument input) override {
        documents.push_back(input);
        return input;
    }

    UploadedDocument update(string id, UploadedDocument input) override {
        for (int i = 0; i < static_cast<int>(documents.size()); i++) {
            if (documents[i].getId() == id) {
                documents[i] = input;
                return documents[i];
            }
        }

        return UploadedDocument();
    }

    bool remove(string id) override {
        for (int i = 0; i < static_cast<int>(documents.size()); i++) {
            if (documents[i].getId() == id) {
                documents.erase(documents.begin() + i);
                return true;
            }
        }

        return false;
    }

    vector<ProcessingDraft> findQueueItems() {
        vector<ProcessingDraft> queue;

        for (UploadedDocument document : documents) {
            if (document.hasPendingDrafts()) {
                queue.push_back({
                    document.getId(),
                    documentCategoryToString(document.getCategory()),
                    document.getDraftCount(),
                    "PENDING_REVIEW"
                });
            }
        }

        return queue;
    }

    UploadedDocument findForReview(string documentId, string category) {
        return findById(documentId);
    }

    UploadedDocument saveDocument(UploadedDocument document) {
        UploadedDocument existing = findById(document.getId());

        if (existing.getId().empty()) {
            return create(document);
        }

        return update(document.getId(), document);
    }

    vector<shared_ptr<DocumentDraft>> saveDrafts(
        vector<shared_ptr<DocumentDraft>> drafts
    ) {
        return drafts;
    }

    bool deleteDocument(string documentId) {
        return remove(documentId);
    }
};

// =====================================================
// Mapper Interface and Mapper
// =====================================================

template <typename T, typename D>
class EntityMapper {
public:
    virtual D toDto(T entity) = 0;
    virtual vector<D> toDtoList(vector<T> entities) = 0;
    virtual ~EntityMapper() {}
};

class DocumentMapper : public EntityMapper<UploadedDocument, UploadedDocumentDto> {
public:
    UploadedDocumentDto toDto(UploadedDocument entity) override {
        return {
            entity.getId(),
            entity.getFileName(),
            entity.getFileType(),
            entity.getFileSize(),
            documentCategoryToString(entity.getCategory()),
            entity.getRemarks(),
            entity.getDraftCount()
        };
    }

    vector<UploadedDocumentDto> toDtoList(vector<UploadedDocument> entities) override {
        vector<UploadedDocumentDto> result;

        for (UploadedDocument entity : entities) {
            result.push_back(toDto(entity));
        }

        return result;
    }
};

// =====================================================
// Creational Pattern: Factory Method
// Class: DraftFactory
// =====================================================

class DraftFactory {
public:
    vector<shared_ptr<DocumentDraft>> createDrafts(
        string documentId,
        DocumentCategory category,
        ExtractResult result
    ) {
        if (category == DocumentCategory::PENGHUNI) {
            return createPenghuniDrafts(documentId, result);
        }

        if (category == DocumentCategory::KUARTERS) {
            return createKuartersDrafts(documentId, result);
        }

        if (category == DocumentCategory::BAYARAN) {
            return createBayaranDrafts(documentId, result);
        }

        return createTunggakanDrafts(documentId, result);
    }

    vector<shared_ptr<DocumentDraft>> createPenghuniDrafts(
        string documentId,
        ExtractResult result
    ) {
        return {
            make_shared<ResidentDraft>(
                "DRAFT_RES_001",
                documentId,
                "Ali Bin Abu",
                "660101011111",
                "A-01"
            )
        };
    }

    vector<shared_ptr<DocumentDraft>> createKuartersDrafts(
        string documentId,
        ExtractResult result
    ) {
        return {
            make_shared<QuarterCategoryDraft>(
                "DRAFT_QCAT_001",
                documentId,
                "Class A Quarters",
                "Block A",
                300.00,
                50.00,
                100.00
            ),
            make_shared<UnitDraft>(
                "DRAFT_UNIT_001",
                documentId,
                "A-01",
                UnitStatus::VACANT,
                "DRAFT_QCAT_001"
            )
        };
    }

    vector<shared_ptr<DocumentDraft>> createBayaranDrafts(
        string documentId,
        ExtractResult result
    ) {
        return {
            make_shared<PaymentDraft>(
                "DRAFT_PAY_001",
                documentId,
                "Ali Bin Abu",
                "660101011111",
                "2026-06-05",
                "RCP-001",
                300.00
            )
        };
    }

    vector<shared_ptr<DocumentDraft>> createTunggakanDrafts(
        string documentId,
        ExtractResult result
    ) {
        return {
            make_shared<ArrearsSummaryDraft>(
                "DRAFT_ARR_001",
                documentId,
                "Ali Bin Abu",
                "660101011111",
                250.00
            )
        };
    }
};

// =====================================================
// Behavioral Pattern: Template Method
// Abstract Class: DocumentWorkflow
// =====================================================

class DocumentWorkflow {
protected:
    DocumentRepository& documentRepository;
    DraftFactory draftFactory;

    virtual ValidationResult validateFile(UploadedFile file) {
        if (file.fileName.empty()) {
            return { false, "File name is required." };
        }

        if (file.fileSize <= 0) {
            return { false, "File size must be greater than zero." };
        }

        return { true, "File is valid." };
    }

    virtual ExtractResult extract(UploadedFile file, ParsingMode parsingMode) = 0;

    virtual UploadedDocument createDocument(
        UploadedFile file,
        DocumentCategory category,
        AdminProfile actor
    ) {
        return UploadedDocument(
            "DOC_" + file.fileName,
            file.fileName,
            file.fileType,
            file.fileSize,
            category,
            actor.id,
            "Document uploaded and waiting for review."
        );
    }

    virtual vector<shared_ptr<DocumentDraft>> createDrafts(
        UploadedDocument document,
        ExtractResult result
    ) {
        return draftFactory.createDrafts(
            document.getId(),
            document.getCategory(),
            result
        );
    }

    virtual void logImport(
        UploadedDocument document,
        AdminProfile actor
    ) {
        cout << "Import logged for document "
             << document.getFileName()
             << " by "
             << actor.username
             << endl;
    }

public:
    DocumentWorkflow(DocumentRepository& repository)
        : documentRepository(repository) {
    }

    ProcessingDraft process(
        UploadedFile file,
        ParsingMode parsingMode,
        AdminProfile actor
    ) {
        ValidationResult validation = validateFile(file);

        if (!validation.valid) {
            return {
                "",
                "INVALID",
                0,
                validation.message
            };
        }

        ExtractResult extractResult = extract(file, parsingMode);
        UploadedDocument document =
            createDocument(file, getSupportedCategory(), actor);

        vector<shared_ptr<DocumentDraft>> drafts =
            createDrafts(document, extractResult);

        document.attachDrafts(drafts);
        document.markPendingReview();

        documentRepository.saveDocument(document);
        documentRepository.saveDrafts(drafts);

        logImport(document, actor);

        return {
            document.getId(),
            documentCategoryToString(document.getCategory()),
            document.getDraftCount(),
            "PENDING_REVIEW"
        };
    }

    virtual DocumentCategory getSupportedCategory() const = 0;

    virtual ~DocumentWorkflow() {}
};

// =====================================================
// Concrete Workflows
// =====================================================

class PenghuniDocumentWorkflow : public DocumentWorkflow {
public:
    PenghuniDocumentWorkflow(DocumentRepository& repository)
        : DocumentWorkflow(repository) {
    }

    ExtractResult extract(UploadedFile file, ParsingMode parsingMode) override {
        cout << "Extracting resident data from " << file.fileName << endl;
        return { { "resident-row-1" } };
    }

    DocumentCategory getSupportedCategory() const override {
        return DocumentCategory::PENGHUNI;
    }
};

class KuartersDocumentWorkflow : public DocumentWorkflow {
public:
    KuartersDocumentWorkflow(DocumentRepository& repository)
        : DocumentWorkflow(repository) {
    }

    ExtractResult extract(UploadedFile file, ParsingMode parsingMode) override {
        cout << "Extracting quarters data from " << file.fileName << endl;
        return { { "quarter-row-1", "unit-row-1" } };
    }

    DocumentCategory getSupportedCategory() const override {
        return DocumentCategory::KUARTERS;
    }
};

class BayaranDocumentWorkflow : public DocumentWorkflow {
public:
    BayaranDocumentWorkflow(DocumentRepository& repository)
        : DocumentWorkflow(repository) {
    }

    ExtractResult extract(UploadedFile file, ParsingMode parsingMode) override {
        cout << "Extracting payment data from " << file.fileName << endl;
        return { { "payment-row-1" } };
    }

    DocumentCategory getSupportedCategory() const override {
        return DocumentCategory::BAYARAN;
    }
};

class TunggakanDocumentWorkflow : public DocumentWorkflow {
public:
    TunggakanDocumentWorkflow(DocumentRepository& repository)
        : DocumentWorkflow(repository) {
    }

    ExtractResult extract(UploadedFile file, ParsingMode parsingMode) override {
        cout << "Extracting arrears data from " << file.fileName << endl;
        return { { "arrears-row-1" } };
    }

    DocumentCategory getSupportedCategory() const override {
        return DocumentCategory::TUNGGAKAN;
    }
};

// =====================================================
// Creational Pattern: Factory Method
// Class: DocumentWorkflowFactory
// =====================================================

class DocumentWorkflowFactory {
private:
    DocumentRepository& documentRepository;

public:
    DocumentWorkflowFactory(DocumentRepository& repository)
        : documentRepository(repository) {
    }

    unique_ptr<DocumentWorkflow> createWorkflow(DocumentCategory kind) {
        if (kind == DocumentCategory::PENGHUNI) {
            return make_unique<PenghuniDocumentWorkflow>(documentRepository);
        }

        if (kind == DocumentCategory::KUARTERS) {
            return make_unique<KuartersDocumentWorkflow>(documentRepository);
        }

        if (kind == DocumentCategory::BAYARAN) {
            return make_unique<BayaranDocumentWorkflow>(documentRepository);
        }

        return make_unique<TunggakanDocumentWorkflow>(documentRepository);
    }

    bool supports(DocumentCategory kind) {
        return kind == DocumentCategory::PENGHUNI ||
               kind == DocumentCategory::KUARTERS ||
               kind == DocumentCategory::BAYARAN ||
               kind == DocumentCategory::TUNGGAKAN;
    }
};

// =====================================================
// Behavioral Pattern: Strategy
// Interface: IVerificationStrategy
// =====================================================

class IVerificationStrategy {
protected:
    DocumentRepository& documentRepository;

public:
    IVerificationStrategy(DocumentRepository& repository)
        : documentRepository(repository) {
    }

    virtual VerificationResult verify(
        string documentId,
        vector<string> selectedKeys,
        AdminProfile actor
    ) = 0;

    virtual bool supports(DocumentCategory category) = 0;

    virtual ~IVerificationStrategy() {}
};

// =====================================================
// Concrete Verification Strategies
// =====================================================

class PenghuniVerificationStrategy : public IVerificationStrategy {
public:
    PenghuniVerificationStrategy(DocumentRepository& repository)
        : IVerificationStrategy(repository) {
    }

    VerificationResult verify(
        string documentId,
        vector<string> selectedKeys,
        AdminProfile actor
    ) override {
        UploadedDocument document = documentRepository.findById(documentId);
        int verifiedCount = 0;

        VerificationContext context = { documentId, actor };

        for (shared_ptr<DocumentDraft> draft : document.getDrafts()) {
            ResidentDraft* residentDraft =
                dynamic_cast<ResidentDraft*>(draft.get());

            if (residentDraft != nullptr && draft->validate().valid) {
                residentDraft->verify(context);
                verifiedCount++;
            }
        }

        document.markVerified("Resident drafts verified.");
        documentRepository.saveDocument(document);

        return {
            true,
            "Resident document verified successfully.",
            verifiedCount
        };
    }

    bool supports(DocumentCategory category) override {
        return category == DocumentCategory::PENGHUNI;
    }
};

class KuartersVerificationStrategy : public IVerificationStrategy {
public:
    KuartersVerificationStrategy(DocumentRepository& repository)
        : IVerificationStrategy(repository) {
    }

    VerificationResult verify(
        string documentId,
        vector<string> selectedKeys,
        AdminProfile actor
    ) override {
        UploadedDocument document = documentRepository.findById(documentId);
        int verifiedCount = 0;

        VerificationContext context = { documentId, actor };

        for (shared_ptr<DocumentDraft> draft : document.getDrafts()) {
            QuarterCategoryDraft* categoryDraft =
                dynamic_cast<QuarterCategoryDraft*>(draft.get());

            UnitDraft* unitDraft =
                dynamic_cast<UnitDraft*>(draft.get());

            if (categoryDraft != nullptr && draft->validate().valid) {
                categoryDraft->verify(context);
                verifiedCount++;
            }

            if (unitDraft != nullptr && draft->validate().valid) {
                unitDraft->verify(context);
                verifiedCount++;
            }
        }

        document.markVerified("Quarters drafts verified.");
        documentRepository.saveDocument(document);

        return {
            true,
            "Quarters document verified successfully.",
            verifiedCount
        };
    }

    bool supports(DocumentCategory category) override {
        return category == DocumentCategory::KUARTERS;
    }
};

class BayaranVerificationStrategy : public IVerificationStrategy {
public:
    BayaranVerificationStrategy(DocumentRepository& repository)
        : IVerificationStrategy(repository) {
    }

    VerificationResult verify(
        string documentId,
        vector<string> selectedKeys,
        AdminProfile actor
    ) override {
        UploadedDocument document = documentRepository.findById(documentId);
        int verifiedCount = 0;

        VerificationContext context = { documentId, actor };

        for (shared_ptr<DocumentDraft> draft : document.getDrafts()) {
            PaymentDraft* paymentDraft =
                dynamic_cast<PaymentDraft*>(draft.get());

            if (paymentDraft != nullptr && draft->validate().valid) {
                paymentDraft->verify(context);
                verifiedCount++;
            }
        }

        document.markVerified("Payment drafts verified.");
        documentRepository.saveDocument(document);

        return {
            true,
            "Payment document verified successfully.",
            verifiedCount
        };
    }

    bool supports(DocumentCategory category) override {
        return category == DocumentCategory::BAYARAN;
    }
};

class TunggakanVerificationStrategy : public IVerificationStrategy {
public:
    TunggakanVerificationStrategy(DocumentRepository& repository)
        : IVerificationStrategy(repository) {
    }

    VerificationResult verify(
        string documentId,
        vector<string> selectedKeys,
        AdminProfile actor
    ) override {
        UploadedDocument document = documentRepository.findById(documentId);
        int verifiedCount = 0;

        VerificationContext context = { documentId, actor };

        for (shared_ptr<DocumentDraft> draft : document.getDrafts()) {
            ArrearsSummaryDraft* arrearsDraft =
                dynamic_cast<ArrearsSummaryDraft*>(draft.get());

            if (arrearsDraft != nullptr && draft->validate().valid) {
                arrearsDraft->verify(context);
                verifiedCount++;
            }
        }

        document.markVerified("Arrears drafts verified.");
        documentRepository.saveDocument(document);

        return {
            true,
            "Arrears document verified successfully.",
            verifiedCount
        };
    }

    bool supports(DocumentCategory category) override {
        return category == DocumentCategory::TUNGGAKAN;
    }
};

// =====================================================
// Creational Pattern: Factory Method
// Class: VerificationStrategyFactory
// =====================================================

class VerificationStrategyFactory {
private:
    DocumentRepository& documentRepository;

public:
    VerificationStrategyFactory(DocumentRepository& repository)
        : documentRepository(repository) {
    }

    unique_ptr<IVerificationStrategy> createStrategy(DocumentCategory kind) {
        if (kind == DocumentCategory::PENGHUNI) {
            return make_unique<PenghuniVerificationStrategy>(documentRepository);
        }

        if (kind == DocumentCategory::KUARTERS) {
            return make_unique<KuartersVerificationStrategy>(documentRepository);
        }

        if (kind == DocumentCategory::BAYARAN) {
            return make_unique<BayaranVerificationStrategy>(documentRepository);
        }

        return make_unique<TunggakanVerificationStrategy>(documentRepository);
    }
};

// =====================================================
// Services
// =====================================================

class UploadedDocumentService {
private:
    DocumentRepository& documentRepository;
    DocumentWorkflowFactory workflowFactory;

public:
    UploadedDocumentService(DocumentRepository& repository)
        : documentRepository(repository),
          workflowFactory(repository) {
    }

    UploadedDocument createUploadedDocumentForKind(
        DocumentCategory kind,
        CreateUploadedDocumentPayload payload,
        AdminProfile actor
    ) {
        UploadedFile file = {
            payload.fileName,
            payload.fileType,
            payload.fileSize
        };

        unique_ptr<DocumentWorkflow> workflow =
            workflowFactory.createWorkflow(kind);

        ProcessingDraft processingDraft =
            workflow->process(file, payload.parsingMode, actor);

        return documentRepository.findById(processingDraft.documentId);
    }

    vector<ProcessingDraft> getProcessingQueue() {
        return documentRepository.findQueueItems();
    }

    UploadedDocument getDocumentForReview(
        string documentId,
        DocumentCategory kind
    ) {
        return documentRepository.findForReview(
            documentId,
            documentCategoryToString(kind)
        );
    }

    shared_ptr<DocumentDraft> updateDraft(
        string documentId,
        DocumentCategory kind,
        DraftUpdateInput input,
        AdminProfile actor
    ) {
        UploadedDocument document =
            documentRepository.findById(documentId);

        for (shared_ptr<DocumentDraft> draft : document.getDrafts()) {
            draft->update(input);
            cout << "Draft updated by " << actor.username << endl;
            return draft;
        }

        return nullptr;
    }

    bool deleteDraft(
        string documentId,
        DocumentCategory kind,
        string draftId,
        AdminProfile actor
    ) {
        UploadedDocument document =
            documentRepository.findById(documentId);

        for (shared_ptr<DocumentDraft> draft : document.getDrafts()) {
            if (draft->getId() == draftId) {
                draft->removeDraft();
                cout << "Draft deleted by " << actor.username << endl;
                return true;
            }
        }

        return false;
    }

    bool deleteUploadedDocument(
        string documentId,
        AdminProfile actor
    ) {
        cout << "Document deleted by " << actor.username << endl;
        return documentRepository.deleteDocument(documentId);
    }
};

class DocumentVerificationService {
private:
    DocumentRepository& documentRepository;
    VerificationStrategyFactory strategyFactory;

public:
    DocumentVerificationService(DocumentRepository& repository)
        : documentRepository(repository),
          strategyFactory(repository) {
    }

    VerificationResult verifyUploadedDocumentForKind(
        DocumentCategory kind,
        string documentId,
        vector<string> selectedKeys,
        AdminProfile actor
    ) {
        unique_ptr<IVerificationStrategy> strategy =
            strategyFactory.createStrategy(kind);

        return strategy->verify(documentId, selectedKeys, actor);
    }

    VerificationResult verifyPenghuniDrafts(
        string documentId,
        vector<string> selectedKeys,
        AdminProfile actor
    ) {
        return verifyUploadedDocumentForKind(
            DocumentCategory::PENGHUNI,
            documentId,
            selectedKeys,
            actor
        );
    }

    VerificationResult verifyKuartersDrafts(
        string documentId,
        vector<string> selectedKeys,
        AdminProfile actor
    ) {
        return verifyUploadedDocumentForKind(
            DocumentCategory::KUARTERS,
            documentId,
            selectedKeys,
            actor
        );
    }

    VerificationResult verifyBayaranDrafts(
        string documentId,
        vector<string> selectedKeys,
        AdminProfile actor
    ) {
        return verifyUploadedDocumentForKind(
            DocumentCategory::BAYARAN,
            documentId,
            selectedKeys,
            actor
        );
    }

    VerificationResult verifyTunggakanDrafts(
        string documentId,
        vector<string> selectedKeys,
        AdminProfile actor
    ) {
        return verifyUploadedDocumentForKind(
            DocumentCategory::TUNGGAKAN,
            documentId,
            selectedKeys,
            actor
        );
    }
};

// =====================================================
// Structural Pattern: Facade
// Class: DocumentProcessingFacade
// =====================================================

class DocumentProcessingFacade {
private:
    UploadedDocumentService& uploadedDocumentService;
    DocumentVerificationService& documentVerificationService;

public:
    DocumentProcessingFacade(
        UploadedDocumentService& uploadedDocumentService,
        DocumentVerificationService& documentVerificationService
    ) : uploadedDocumentService(uploadedDocumentService),
        documentVerificationService(documentVerificationService) {
    }

    ProcessingDraft extractAndStoreFile(
        UploadedFile file,
        DocumentCategory kind,
        ParsingMode parsingMode,
        AdminProfile actor
    ) {
        UploadedDocument document =
            uploadedDocumentService.createUploadedDocumentForKind(
                kind,
                {
                    file.fileName,
                    file.fileType,
                    file.fileSize,
                    "Uploaded through document processing facade.",
                    parsingMode
                },
                actor
            );

        return {
            document.getId(),
            documentCategoryToString(document.getCategory()),
            document.getDraftCount(),
            "PENDING_REVIEW"
        };
    }

    UploadedDocument getReviewDocument(
        string documentId,
        DocumentCategory kind
    ) {
        return uploadedDocumentService.getDocumentForReview(documentId, kind);
    }

    shared_ptr<DocumentDraft> updateDraft(
        string documentId,
        DocumentCategory kind,
        DraftUpdateInput input
    ) {
        AdminProfile systemActor = { "SYSTEM", "System" };

        return uploadedDocumentService.updateDraft(
            documentId,
            kind,
            input,
            systemActor
        );
    }

    VerificationResult verifyDocument(
        string documentId,
        DocumentCategory kind,
        vector<string> selectedKeys,
        AdminProfile actor
    ) {
        return documentVerificationService.verifyUploadedDocumentForKind(
            kind,
            documentId,
            selectedKeys,
            actor
        );
    }
};

// =====================================================
// Controller
// =====================================================

class UploadedDocumentController {
private:
    UploadedDocumentService& uploadedDocumentService;
    DocumentProcessingFacade& documentProcessingFacade;
    DocumentMapper documentMapper;

public:
    UploadedDocumentController(
        UploadedDocumentService& uploadedDocumentService,
        DocumentProcessingFacade& documentProcessingFacade
    ) : uploadedDocumentService(uploadedDocumentService),
        documentProcessingFacade(documentProcessingFacade) {
    }

    ApiResponse<vector<ProcessingDraft>> getUploadedDocuments(
        UploadedDocumentListRequest request
    ) {
        vector<ProcessingDraft> queue =
            uploadedDocumentService.getProcessingQueue();

        return {
            true,
            "Processing queue retrieved successfully.",
            queue
        };
    }

    ApiResponse<ProcessingDraft> postUploadDocument(
        UploadedDocumentCreateRequest request
    ) {
        UploadedFile file = {
            request.fileName,
            request.fileType,
            request.fileSize
        };

        ProcessingDraft draft =
            documentProcessingFacade.extractAndStoreFile(
                file,
                request.category,
                request.parsingMode,
                request.actor
            );

        return {
            true,
            "Document uploaded and drafts created successfully.",
            draft
        };
    }

    ApiResponse<UploadedDocumentDto> getDocumentForReview(
        string documentId,
        DocumentCategory category
    ) {
        UploadedDocument document =
            documentProcessingFacade.getReviewDocument(documentId, category);

        if (document.getId().empty()) {
            return {
                false,
                "Document not found.",
                UploadedDocumentDto()
            };
        }

        return {
            true,
            "Document retrieved for review.",
            documentMapper.toDto(document)
        };
    }

    ApiResponse<string> patchDraft(DraftUpdateRequest request) {
        shared_ptr<DocumentDraft> draft =
            uploadedDocumentService.updateDraft(
                request.documentId,
                DocumentCategory::PENGHUNI,
                { request.newRemark },
                request.actor
            );

        if (draft == nullptr) {
            return {
                false,
                "Draft update failed.",
                ""
            };
        }

        return {
            true,
            "Draft updated successfully.",
            draft->getId()
        };
    }

    ApiResponse<VerificationResult> postVerifyDocument(
        DocumentVerifyRequest request
    ) {
        VerificationResult result =
            documentProcessingFacade.verifyDocument(
                request.documentId,
                request.category,
                request.selectedKeys,
                request.actor
            );

        return {
            result.success,
            result.message,
            result
        };
    }

    ApiResponse<bool> deleteUploadedDocument(
        UploadedDocumentDeleteRequest request
    ) {
        bool deleted =
            uploadedDocumentService.deleteUploadedDocument(
                request.documentId,
                request.actor
            );

        return {
            deleted,
            deleted ? "Document deleted successfully." : "Document deletion failed.",
            deleted
        };
    }
};

// =====================================================
// Main Function
// =====================================================

int main() {
    DocumentRepository documentRepository;

    UploadedDocumentService uploadedDocumentService(documentRepository);
    DocumentVerificationService documentVerificationService(documentRepository);

    DocumentProcessingFacade documentProcessingFacade(
        uploadedDocumentService,
        documentVerificationService
    );

    UploadedDocumentController uploadedDocumentController(
        uploadedDocumentService,
        documentProcessingFacade
    );

    AdminProfile admin = {
        "ADMIN_001",
        "documentAdmin"
    };

    UploadedDocumentCreateRequest uploadRequest = {
        "penghuni_jun_2026.xlsx",
        "xlsx",
        2048,
        DocumentCategory::PENGHUNI,
        "Monthly resident upload",
        ParsingMode::EXCEL,
        admin
    };

    ApiResponse<ProcessingDraft> uploadResponse =
        uploadedDocumentController.postUploadDocument(uploadRequest);

    cout << "\n===== Upload Result =====" << endl;
    cout << uploadResponse.message << endl;
    cout << "Document ID: " << uploadResponse.data.documentId << endl;
    cout << "Category: " << uploadResponse.data.category << endl;
    cout << "Draft Count: " << uploadResponse.data.draftCount << endl;
    cout << "Status: " << uploadResponse.data.status << endl;

    ApiResponse<UploadedDocumentDto> reviewResponse =
        uploadedDocumentController.getDocumentForReview(
            uploadResponse.data.documentId,
            DocumentCategory::PENGHUNI
        );

    cout << "\n===== Review Document =====" << endl;
    cout << reviewResponse.message << endl;
    cout << "File Name: " << reviewResponse.data.fileName << endl;
    cout << "Draft Count: " << reviewResponse.data.draftCount << endl;

    DocumentVerifyRequest verifyRequest = {
        uploadResponse.data.documentId,
        DocumentCategory::PENGHUNI,
        { "DRAFT_RES_001" },
        admin
    };

    ApiResponse<VerificationResult> verifyResponse =
        uploadedDocumentController.postVerifyDocument(verifyRequest);

    cout << "\n===== Verification Result =====" << endl;
    cout << verifyResponse.message << endl;
    cout << "Verified Count: " << verifyResponse.data.verifiedCount << endl;

    ApiResponse<vector<ProcessingDraft>> queueResponse =
        uploadedDocumentController.getUploadedDocuments({ "" });

    cout << "\n===== Processing Queue =====" << endl;

    for (ProcessingDraft item : queueResponse.data) {
        cout << "Document ID: " << item.documentId
             << " | Category: " << item.category
             << " | Draft Count: " << item.draftCount
             << " | Status: " << item.status
             << endl;
    }

    return 0;
}
