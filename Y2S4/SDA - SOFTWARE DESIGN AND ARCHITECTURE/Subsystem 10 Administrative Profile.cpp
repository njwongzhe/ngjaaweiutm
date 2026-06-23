#include <iostream>
#include <string>
#include <vector>

using namespace std;

// =====================================================
// Enumerations
// =====================================================

enum class DocumentCategory {
    BAYARAN,
    TUNGGAKAN,
    PENGHUNI,
    KUARTERS
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

// =====================================================
// DTOs and Request Models
// =====================================================

template <typename T>
struct ApiResponse {
    bool success;
    string message;
    T data;
};

struct AuthSession {
    string sessionToken;
    bool authenticated;
};

struct AuthUser {
    string userId;
    string email;
};

struct AuthenticatedRequest {
    string adminId;
    string sessionToken;
};

struct AdminProfileUpdateInput {
    string fullName;
    string phoneNumber;
    string department;
    string gender;
};

struct ProfileUpdateRequest {
    string adminId;
    string sessionToken;
    AdminProfileUpdateInput input;
};

struct ChangePasswordRequest {
    string adminId;
    string sessionToken;
    string currentPassword;
    string newPassword;
};

struct ResetSystemRequest {
    string adminId;
    string sessionToken;
    string confirmationText;
};

struct ResetSystemResult {
    bool success;
    string message;
    int affectedRecords;
};

// =====================================================
// Mock External Supabase Client
// =====================================================
// This class simulates the external Supabase authentication client.
// In the real system, this would be replaced by Supabase SDK/API calls.

class SupabaseClient {
private:
    string clientName;

public:
    SupabaseClient(string clientName) {
        this->clientName = clientName;
    }

    AuthSession signInWithPassword(string email, string password) {
        cout << "[" << clientName << "] Login request sent to Supabase Auth." << endl;

        if (email.empty() || password.empty()) {
            return { "", false };
        }

        return { "SESSION_TOKEN_123", true };
    }

    bool signOut(string sessionToken) {
        cout << "[" << clientName << "] Logout request sent to Supabase Auth." << endl;
        return !sessionToken.empty();
    }

    AuthUser getUserBySession(string sessionToken) {
        cout << "[" << clientName << "] Current user request sent to Supabase Auth." << endl;

        if (sessionToken.empty()) {
            return { "", "" };
        }

        return { "ADMIN_001", "admin@johor.gov.my" };
    }

    bool updatePassword(string userId, string newPassword) {
        cout << "[" << clientName << "] Password update request sent to Supabase Auth." << endl;

        return !userId.empty() && newPassword.length() >= 8;
    }
};

// =====================================================
// Creational Pattern: Singleton
// Class: SupabaseClientProvider
// =====================================================
// The provider creates Supabase clients only once and exposes shared access.

class SupabaseClientProvider {
private:
    SupabaseClient serverClient;
    SupabaseClient adminClient;

    SupabaseClientProvider()
        : serverClient("Server Supabase Client"),
          adminClient("Admin Supabase Client") {
    }

public:
    SupabaseClientProvider(const SupabaseClientProvider&) = delete;
    SupabaseClientProvider& operator=(const SupabaseClientProvider&) = delete;

    static SupabaseClientProvider& getInstance() {
        static SupabaseClientProvider instance;
        return instance;
    }

    SupabaseClient& getServerClient() {
        return serverClient;
    }

    SupabaseClient& getAdminClient() {
        return adminClient;
    }
};

// =====================================================
// Structural Pattern: Adapter
// Class: SupabaseAuthAdapter
// =====================================================
// The adapter converts Supabase Auth operations into methods used internally
// by ProfileService.

class SupabaseAuthAdapter {
private:
    SupabaseClientProvider& clientProvider;

public:
    SupabaseAuthAdapter()
        : clientProvider(SupabaseClientProvider::getInstance()) {
    }

    AuthSession login(string email, string password) {
        return clientProvider
            .getServerClient()
            .signInWithPassword(email, password);
    }

    bool logout(string sessionToken) {
        return clientProvider
            .getServerClient()
            .signOut(sessionToken);
    }

    AuthUser getCurrentUser(string sessionToken) {
        return clientProvider
            .getServerClient()
            .getUserBySession(sessionToken);
    }

    bool changePassword(string userId, string newPassword) {
        return clientProvider
            .getAdminClient()
            .updatePassword(userId, newPassword);
    }
};

// =====================================================
// Domain Entity
// Class: AdminProfile
// =====================================================

class AdminProfile {
private:
    string id;
    string fullName;
    string email;
    string phoneNumber;
    string department;
    string gender;
    string role;
    bool isActive;

public:
    AdminProfile() {
        id = "";
        fullName = "";
        email = "";
        phoneNumber = "";
        department = "";
        gender = "";
        role = "";
        isActive = false;
    }

    AdminProfile(
        string id,
        string fullName,
        string email,
        string phoneNumber,
        string department,
        string gender,
        string role,
        bool isActive
    ) {
        this->id = id;
        this->fullName = fullName;
        this->email = email;
        this->phoneNumber = phoneNumber;
        this->department = department;
        this->gender = gender;
        this->role = role;
        this->isActive = isActive;
    }

    AdminProfile updateProfile(AdminProfileUpdateInput input) {
        fullName = input.fullName;
        phoneNumber = input.phoneNumber;
        department = input.department;
        gender = input.gender;

        return *this;
    }

    bool changePassword(string currentPassword, string newPassword) {
        if (currentPassword.empty()) {
            return false;
        }

        return newPassword.length() >= 8;
    }

    AdminProfile deactivate(string reason) {
        isActive = false;
        cout << "Admin profile deactivated. Reason: " << reason << endl;

        return *this;
    }

    bool canUploadDocument(DocumentCategory category) {
        if (!isActive) {
            return false;
        }

        if (role == "SUPER_ADMIN") {
            return true;
        }

        if (role == "ADMIN" &&
            (category == DocumentCategory::PENGHUNI ||
             category == DocumentCategory::KUARTERS)) {
            return true;
        }

        if (role == "FINANCE_ADMIN" &&
            (category == DocumentCategory::BAYARAN ||
             category == DocumentCategory::TUNGGAKAN)) {
            return true;
        }

        return false;
    }

    string getDisplayName() {
        if (!fullName.empty()) {
            return fullName;
        }

        return email;
    }

    string getId() const {
        return id;
    }

    string getFullName() const {
        return fullName;
    }

    string getEmail() const {
        return email;
    }

    string getPhoneNumber() const {
        return phoneNumber;
    }

    string getDepartment() const {
        return department;
    }

    string getGender() const {
        return gender;
    }

    string getRole() const {
        return role;
    }

    bool getIsActive() const {
        return isActive;
    }
};

// =====================================================
// Service Layer
// Class: ProfileService
// =====================================================

class ProfileService {
private:
    SupabaseAuthAdapter supabaseAuthAdapter;
    vector<AdminProfile> adminProfiles;

    int findProfileIndex(string adminId) {
        for (int i = 0; i < static_cast<int>(adminProfiles.size()); i++) {
            if (adminProfiles[i].getId() == adminId) {
                return i;
            }
        }

        return -1;
    }

public:
    ProfileService() {
        adminProfiles.push_back(
            AdminProfile(
                "ADMIN_001",
                "Galen GUI",
                "admin@johor.gov.my",
                "0123456789",
                "Finance Department",
                "Male",
                "SUPER_ADMIN",
                true
            )
        );

        adminProfiles.push_back(
            AdminProfile(
                "ADMIN_002",
                "Quarter Officer",
                "quarter@johor.gov.my",
                "0199999999",
                "Quarters Department",
                "Female",
                "ADMIN",
                true
            )
        );
    }

    AdminProfile getProfile(string adminId) {
        int index = findProfileIndex(adminId);

        if (index == -1) {
            return AdminProfile();
        }

        return adminProfiles[index];
    }

    AdminProfile updateProfile(
        string adminId,
        AdminProfileUpdateInput input
    ) {
        int index = findProfileIndex(adminId);

        if (index == -1) {
            return AdminProfile();
        }

        AdminProfile updatedProfile =
            adminProfiles[index].updateProfile(input);

        adminProfiles[index] = updatedProfile;

        return updatedProfile;
    }

    bool changePassword(
        string adminId,
        string currentPassword,
        string newPassword
    ) {
        int index = findProfileIndex(adminId);

        if (index == -1) {
            return false;
        }

        bool localValidation =
            adminProfiles[index].changePassword(
                currentPassword,
                newPassword
            );

        if (!localValidation) {
            return false;
        }

        return supabaseAuthAdapter.changePassword(
            adminId,
            newPassword
        );
    }

    ResetSystemResult resetSystem(
        AdminProfile actor,
        string confirmationText
    ) {
        if (!actor.getIsActive()) {
            return {
                false,
                "Reset failed. The actor profile is inactive.",
                0
            };
        }

        if (actor.getRole() != "SUPER_ADMIN") {
            return {
                false,
                "Reset failed. Only SUPER_ADMIN can reset the system.",
                0
            };
        }

        if (confirmationText != "RESET SYSTEM") {
            return {
                false,
                "Reset failed. Confirmation text is incorrect.",
                0
            };
        }

        cout << "System reset executed by: "
             << actor.getDisplayName() << endl;

        return {
            true,
            "System reset completed successfully.",
            128
        };
    }

    string buildAvatarUrl(string seed, string gender) {
        string style = gender == "Female" ? "female" : "male";

        return "https://avatar.example.com/"
            + style
            + "?seed="
            + seed;
    }
};

// =====================================================
// Controller Layer
// Class: ProfileController
// =====================================================

class ProfileController {
private:
    ProfileService& profileService;
    SupabaseAuthAdapter supabaseAuthAdapter;

    bool isAuthenticated(string sessionToken, string adminId) {
        AuthUser currentUser =
            supabaseAuthAdapter.getCurrentUser(sessionToken);

        return currentUser.userId == adminId;
    }

public:
    ProfileController(ProfileService& profileService)
        : profileService(profileService) {
    }

    ApiResponse<AdminProfile> getProfile(
        AuthenticatedRequest request
    ) {
        if (!isAuthenticated(request.sessionToken, request.adminId)) {
            return {
                false,
                "Profile retrieval failed. User is not authenticated.",
                AdminProfile()
            };
        }

        AdminProfile profile =
            profileService.getProfile(request.adminId);

        if (profile.getId().empty()) {
            return {
                false,
                "Profile not found.",
                AdminProfile()
            };
        }

        return {
            true,
            "Profile retrieved successfully.",
            profile
        };
    }

    ApiResponse<AdminProfile> putProfile(
        ProfileUpdateRequest request
    ) {
        if (!isAuthenticated(request.sessionToken, request.adminId)) {
            return {
                false,
                "Profile update failed. User is not authenticated.",
                AdminProfile()
            };
        }

        AdminProfile profile =
            profileService.updateProfile(
                request.adminId,
                request.input
            );

        if (profile.getId().empty()) {
            return {
                false,
                "Profile update failed. Profile not found.",
                AdminProfile()
            };
        }

        return {
            true,
            "Profile updated successfully.",
            profile
        };
    }

    ApiResponse<bool> postChangePassword(
        ChangePasswordRequest request
    ) {
        if (!isAuthenticated(request.sessionToken, request.adminId)) {
            return {
                false,
                "Password change failed. User is not authenticated.",
                false
            };
        }

        bool changed =
            profileService.changePassword(
                request.adminId,
                request.currentPassword,
                request.newPassword
            );

        return {
            changed,
            changed ? "Password changed successfully."
                    : "Password change failed.",
            changed
        };
    }

    ApiResponse<ResetSystemResult> postResetSystem(
        ResetSystemRequest request
    ) {
        if (!isAuthenticated(request.sessionToken, request.adminId)) {
            return {
                false,
                "System reset failed. User is not authenticated.",
                { false, "Unauthenticated request.", 0 }
            };
        }

        AdminProfile actor =
            profileService.getProfile(request.adminId);

        ResetSystemResult result =
            profileService.resetSystem(
                actor,
                request.confirmationText
            );

        return {
            result.success,
            result.message,
            result
        };
    }
};

// =====================================================
// Main Function
// =====================================================

int main() {
    ProfileService profileService;
    ProfileController profileController(profileService);

    AuthenticatedRequest profileRequest = {
        "ADMIN_001",
        "SESSION_TOKEN_123"
    };

    ApiResponse<AdminProfile> profileResponse =
        profileController.getProfile(profileRequest);

    cout << "\n===== Get Profile =====" << endl;
    cout << profileResponse.message << endl;

    if (profileResponse.success) {
        cout << "Name: "
             << profileResponse.data.getDisplayName() << endl;
        cout << "Email: "
             << profileResponse.data.getEmail() << endl;
        cout << "Department: "
             << profileResponse.data.getDepartment() << endl;
        cout << "Role: "
             << profileResponse.data.getRole() << endl;
    }

    ProfileUpdateRequest updateRequest = {
        "ADMIN_001",
        "SESSION_TOKEN_123",
        {
            "Galen GUI Updated",
            "0188888888",
            "Administrative Department",
            "Male"
        }
    };

    ApiResponse<AdminProfile> updateResponse =
        profileController.putProfile(updateRequest);

    cout << "\n===== Update Profile =====" << endl;
    cout << updateResponse.message << endl;

    if (updateResponse.success) {
        cout << "Updated Name: "
             << updateResponse.data.getDisplayName() << endl;
        cout << "Updated Phone: "
             << updateResponse.data.getPhoneNumber() << endl;
        cout << "Updated Department: "
             << updateResponse.data.getDepartment() << endl;
    }

    ChangePasswordRequest changePasswordRequest = {
        "ADMIN_001",
        "SESSION_TOKEN_123",
        "oldPassword123",
        "newPassword123"
    };

    ApiResponse<bool> passwordResponse =
        profileController.postChangePassword(changePasswordRequest);

    cout << "\n===== Change Password =====" << endl;
    cout << passwordResponse.message << endl;

    ResetSystemRequest resetRequest = {
        "ADMIN_001",
        "SESSION_TOKEN_123",
        "RESET SYSTEM"
    };

    ApiResponse<ResetSystemResult> resetResponse =
        profileController.postResetSystem(resetRequest);

    cout << "\n===== Reset System =====" << endl;
    cout << resetResponse.message << endl;
    cout << "Affected Records: "
         << resetResponse.data.affectedRecords << endl;

    AdminProfile profile =
        profileService.getProfile("ADMIN_001");

    cout << "\n===== Permission Check =====" << endl;
    cout << "Can upload BAYARAN document: "
         << (profile.canUploadDocument(DocumentCategory::BAYARAN) ? "Yes" : "No")
         << endl;
    cout << "Avatar URL: "
         << profileService.buildAvatarUrl(profile.getId(), profile.getGender())
         << endl;

    return 0;
}
