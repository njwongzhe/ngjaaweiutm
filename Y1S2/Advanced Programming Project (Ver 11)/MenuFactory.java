import java.util.Scanner;

public class MenuFactory {
    public static Menu createMenu(Scanner read) {
        while (true) {
            System.out.println(ColourManager.ouColour() + "\n--- Welcome to Group 8 Fast Food Order Record Management System ---"); // Output
            System.out.println(                       "                      Make Your Day With Food!                     ");
            System.out.println(ColourManager.ouColour() + "<Who Are You?>"); // Output
            System.out.println(ColourManager.opColour() + "1. Staff"); // Option
            System.out.println(                           "2. Admin");
            System.out.println(ColourManager.ouColour() + "<Setting>"); // Output
            System.out.println(ColourManager.opColour() + "3. CLI Colour Customization" + ColourManager.reColour()); // Option

            System.out.printf(ColourManager.ouColour() + "Select an Option (1 - 3): " + ColourManager.reColour());

            try {
                System.out.print(ColourManager.inColour()); 
                int choice = Integer.parseInt(read.nextLine().trim());
                System.out.print(ColourManager.reColour());

                if (choice == 3) {
                    ColourManager.customizeColour(read);
                    continue;
                }

                switch (choice) {
                    case 1:
                        System.out.println(ColourManager.ouColour() + "\n==============================================" + ColourManager.reColour()); // Output
                        return StaffMenu.loginOrSignup(read);
                    case 2:
                        System.out.println(ColourManager.ouColour() + "\n==============================================" + ColourManager.reColour()); // Output
                        return AdminMenu.loginOrSignup(read);
                    default:
                        System.out.println(ColourManager.erColour() + "Only inputs 1 to 3 are accepted. Please try again.\n" + ColourManager.reColour()); // Error
                }
            } catch (NumberFormatException e) {
                System.out.println(ColourManager.erColour() + "Invalid input. Please try again.\n" + ColourManager.reColour()); // Error
            }
        }
    }
}