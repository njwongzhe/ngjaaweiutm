import java.util.ArrayList;
import java.util.Scanner;
import javax.swing.JFrame;
import javax.swing.JOptionPane;
import javax.swing.JPasswordField;
import javax.swing.SwingUtilities;

public class Staff {
    private String name;
    private String password;

    private static final int REGISTER_CODE = 8888;

    public Staff(String name, String password) {
        this.name = name;
        this.password = password;
    }

    public static Staff readRegistrationInfo(Scanner read, ArrayList<Staff> existingStaff) {
        // Get name with duplicate check
        String name;
        while (true) {
            System.out.printf(ColourManager.ouColour() + "Enter Staff Name: " + ColourManager.reColour());
            System.out.print(ColourManager.inColour());
            name = read.nextLine().trim();
            System.out.print(ColourManager.reColour());
            
            if (name.isEmpty()) {
                System.out.println(ColourManager.erColour() + "Name cannot be empty!\n" + ColourManager.reColour());
                continue;
            }
            
            // Check for existing username (non-case sensitive)
            boolean exists = false;
            for (Staff staff : existingStaff) {
                if (staff.getName().equals(name)) {
                    exists = true;
                    break;
                }
            }
            
            if (exists) {
                System.out.println(ColourManager.erColour() + "Username already exists! Please choose another." + ColourManager.reColour());
                System.out.printf(ColourManager.ouColour() + "Try Again? (y/n): " + ColourManager.reColour());
                System.out.print(ColourManager.inColour());
                if (!read.nextLine().toLowerCase().equals("y")) {
                    System.out.print(ColourManager.reColour());
                    return null; // User chose not to retry 
                }
            } else {
                break; // Valid unique name
            }
        }

        // Get password
        String password = readPasswordWithGUI();
        if (password == null) {
            System.out.println(ColourManager.suColour() + "Password input cancelled." + ColourManager.reColour());
            return null;
        }

        // Verify registration code
        System.out.printf(ColourManager.ouColour() + "Enter registration code: " + ColourManager.reColour());
        System.out.print(ColourManager.inColour());
        String codeInput = read.nextLine();
        System.out.print(ColourManager.reColour());

        try {
            int code = Integer.parseInt(codeInput);
            if (code == REGISTER_CODE) {
                return new Staff(name, password);  // registration successful
            } else {
                System.out.println(ColourManager.erColour() + "Invalid registration code." + ColourManager.reColour());
                return null;
            }
        } catch (NumberFormatException e) {
            System.out.println(ColourManager.erColour() + "Invalid input! Registration code must be a number." + ColourManager.reColour());
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

    public static Staff fromCSVString(String line) {
        String[] parts = line.split(",", -1);
        if (parts.length != 2) return null;
        return new Staff(unescapeComma(parts[0]), unescapeComma(parts[1]));
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

    public void printStaffInfo() {
        System.out.println("Staff Name: " + name);
        System.out.println("Password  : " + password);
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj) return true;
        if (obj == null || getClass() != obj.getClass()) return false;
        Staff other = (Staff) obj;
        return name.toLowerCase().equals(other.name.toLowerCase());
    }

    @Override
    public int hashCode() {
        return name.toLowerCase().hashCode();
    }
}