import java.util.ArrayList;
import java.util.Scanner;
import javax.swing.JFrame;
import javax.swing.JOptionPane;
import javax.swing.JPasswordField;
import javax.swing.SwingUtilities;

public class Admin {
    private String name;
    private String password;

    private static final int REGISTER_CODE = 9999;

    public Admin(String name, String password) {
        this.name = name;
        this.password = password;
    }

    public static Admin readRegistrationInfo(Scanner read, ArrayList<Admin> existingAdmins) {
        // Get name with duplicate check
        String name;
        while (true) {
            System.out.printf("Enter admin name: ");
            name = read.nextLine().trim();
            
            if (name.isEmpty()) {
                System.out.println("Name cannot be empty!");
                continue;
            }
            
            // Check for existing username (non-case insensitive)
            boolean exists = false;
            for (Admin admin : existingAdmins) {
                if (admin.getName().equals(name)) {
                    exists = true;
                    break;
                }
            }
            
            if (exists) {
                System.out.println("Username already exists! Please choose another.");
                System.out.printf("Try again? (y/n): ");
                if (!read.nextLine().toLowerCase().equals("y")) {
                    return null; // User chose not to retry
                }
            } else {
                break; // Valid unique name
            }
        }

        // Get password
        String password = readPasswordWithGUI();
        if (password == null) {
            System.out.println("Password input cancelled.");
            return null;
        }

        // Verify registration code
        System.out.printf("Enter registration code: ");
        String codeInput = read.nextLine();

        try {
            int code = Integer.parseInt(codeInput);
            if (code == REGISTER_CODE) {
                return new Admin(name, password);  // registration successful
            } else {
                System.out.println("Invalid registration code.");
                return null;
            }
        } catch (NumberFormatException e) {
            System.out.println("Invalid input! Registration code must be a number.");
            return null;
        }
    }

    public static String readPasswordWithGUI() {
        JPasswordField passwordField = new JPasswordField(20);
        
        // Schedule focus request when dialog is visible
        SwingUtilities.invokeLater(() -> passwordField.requestFocusInWindow());

        // Create a parent frame that's always on top
        JFrame frame = new JFrame();
        frame.setAlwaysOnTop(true);
        
        while (true) {
            int option = JOptionPane.showConfirmDialog(
                frame,  // Use our always-on-top frame as parent
                passwordField, 
                "Enter Password", 
                JOptionPane.OK_CANCEL_OPTION,
                JOptionPane.PLAIN_MESSAGE
            );

            if (option == JOptionPane.OK_OPTION) {
                char[] passwordChars = passwordField.getPassword();
                String passwordStr = new String(passwordChars).trim();
                java.util.Arrays.fill(passwordChars, ' ');

                if (!passwordStr.isEmpty()) {
                    frame.dispose();  // Clean up the frame
                    return passwordStr;
                } else {
                    JOptionPane.showMessageDialog(
                        frame,
                        "Password cannot be empty. Please try again.",
                        "Error",
                        JOptionPane.ERROR_MESSAGE
                    );
                }
            } else {
                frame.dispose();  // Clean up the frame
                return null;
            }
        }
    }

    public String toCSVString() {
        return escapeComma(name) + "," + escapeComma(password);
    }

    public static Admin fromCSVString(String line) {
        String[] parts = line.split(",", -1);
        if (parts.length != 2) return null;
        return new Admin(unescapeComma(parts[0]), unescapeComma(parts[1]));
    }

    private static String escapeComma(String value) {
        return value.replace(",", "&#44;");
    }

    private static String unescapeComma(String value) {
        return value.replace("&#44;", ",");
    }

    public String getName() {
        return name;
    }

    public String getPassword() {
        return password;
    }

    public void printAdminInfo() {
        System.out.println("Admin Name: " + name);
        System.out.println("Password  : " + password);
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj) return true;
        if (obj == null || getClass() != obj.getClass()) return false;
        Admin other = (Admin) obj;
        return name.toLowerCase().equals(other.name.toLowerCase());
    }

    @Override
    public int hashCode() {
        return name.toLowerCase().hashCode();
    }
}
