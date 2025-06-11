import java.util.Scanner;

public class ColourManager {
    public static final String RESET = "\u001B[0m";
    public static final String RED = "\u001B[31m";
    public static final String ORANGE = "\u001B[38;5;208m"; // ANSI 256-color orange
    public static final String YELLOW = "\u001B[33m";
    public static final String GREEN = "\u001B[32m";
    public static final String BLUE = "\u001B[34m";
    public static final String INDIGO = "\u001B[38;5;54m";   // Close to indigo
    public static final String VIOLET = "\u001B[35m";        // Purple as violet
    public static final String CYAN = "\u001B[36m";
    public static final String WHITE = "\u001B[37m";
    public static String currentOutputColour = ORANGE;
    public static String currentInputColour = BLUE;
    public static String currentOptionColour = WHITE;
    public static String currentSuccessColour = GREEN;
    public static String currentErrorColour = RED;

    // Colour.ouColour()
    public static String ouColour() {
        return currentOutputColour;
    }

    // Colour.inColour()
    public static String inColour() {
        return currentInputColour;
    }

    // Colour.suColour()
    public static String suColour() {
        return currentSuccessColour;
    }

    // Colour.opColour()
    public static String opColour() {
        return currentOptionColour;
    }

    // Colour.erColour()
    public static String erColour() {
        return currentErrorColour;
    }

    // Colour.reColour()
    public static String reColour() {
        return RESET;
    }

    public static void customizeColour(Scanner read) {
        while (true) {
            System.out.println(currentOutputColour + "\n<CLI Colour Customization>"); // Output
            System.out.println(currentOptionColour + "1. Change System Main Colour       -> " + currentOutputColour + "Current Colour Sample Text" + RESET); // Option
            System.out.println(currentOptionColour + "2. Change Input Colour             -> " + currentInputColour + "Current Colour Sample Text" + RESET);
            System.out.println(currentOptionColour + "3. Change Avaliable Option Colour  -> " + currentOptionColour + "Current Colour Sample Text" + RESET);
            System.out.println(currentOptionColour + "4. Change Operation Success Colour -> " + currentSuccessColour + "Current Colour Sample Text" + RESET);
            System.out.println(currentOptionColour + "5. Change Operation Error Colour   -> " + currentErrorColour + "Current Colour Sample Text " + RESET);
            System.out.println(currentOptionColour + "6. Reset to Default Colors");
            System.out.println(currentOptionColour + "0. Save and Exit");
            
            System.out.print(currentOutputColour + "Select an Option (0 - 6): " + RESET); // Ask Input
            System.out.print(currentInputColour);
            String input = read.nextLine().trim(); // Input
            System.out.print(RESET);
            
            try {
                int choice = Integer.parseInt(input);
                
                if (choice == 0) {
                    FileManager.saveColourSettings();
                    System.out.println(currentSuccessColour + "Color settings saved!" + RESET); // Success
                    return;
                } else if (choice == 6) {
                    resetToDefaults();
                    System.out.println(currentSuccessColour + "Colors reset to defaults!" + RESET); // Success
                    continue;
                } else if (choice >= 1 && choice <= 5) {
                    System.out.println(currentOutputColour + "\n------------- Change Colors -------------"); // Output
                    System.out.println(currentOutputColour + "\nAvailable Colors:"); // Output
                    System.out.println(currentOptionColour + "1. Red    -> " + RED + "Sample" + RESET); // Option
                    System.out.println(currentOptionColour + "2. Orange -> " + ORANGE + "Sample" + RESET);
                    System.out.println(currentOptionColour + "3. Yellow -> " + YELLOW + "Sample" + RESET);
                    System.out.println(currentOptionColour + "4. Green  -> " + GREEN + "Sample" + RESET);
                    System.out.println(currentOptionColour + "5. Blue   -> " + BLUE + "Sample" + RESET);
                    System.out.println(currentOptionColour + "6. Indigo -> " + INDIGO + "Sample" + RESET);
                    System.out.println(currentOptionColour + "7. Violet -> " + VIOLET + "Sample" + RESET);
                    System.out.println(currentOptionColour + "8. Cyan   -> " + CYAN + "Sample" + RESET);
                    System.out.println(currentOptionColour + "9. White  -> " + WHITE + "Sample" + RESET);
                    System.out.print(currentOutputColour + "Select Color (1 - 9): " + RESET); // Ask Input
                    System.out.print(currentInputColour);
                    String colorChoice = read.nextLine().trim(); // Input
                    System.out.print(RESET);
                    
                    try {
                        int colorNum = Integer.parseInt(colorChoice);
                        String selectedColor = getColorByNumber(colorNum);
                        if (selectedColor != null) {
                            switch (choice) {
                                case 1: currentOutputColour = selectedColor; break;
                                case 2: currentInputColour = selectedColor; break;
                                case 3: currentOptionColour = selectedColor; break;
                                case 4: currentSuccessColour = selectedColor; break;
                                case 5: currentErrorColour = selectedColor; break;
                            }
                            System.out.println(currentSuccessColour + "Color updated!" + RESET); // Success
                        } else {
                            System.out.println(currentErrorColour + "Invalid color choice!" + RESET); // Error
                        }
                    } catch (NumberFormatException e) {
                        System.out.println(currentErrorColour + "Please enter a number!" + RESET); // Error
                    }
                } else {
                    System.out.println(currentErrorColour + "Invalid option!" + RESET); // Error
                }
            } catch (NumberFormatException e) {
                System.out.println(currentErrorColour + "Please enter a number!" + RESET); // Error
            }
            System.out.println(currentOutputColour + "\n-----------------------------------------" + RESET);
        }
    }

    private static void resetToDefaults() {
        currentOutputColour = ORANGE;
        currentInputColour = BLUE;
        currentOptionColour = WHITE;
        currentSuccessColour = GREEN;
        currentErrorColour = RED;
    }

    private static String getColorByNumber(int num) {
        switch (num) {
            case 1: return RED;
            case 2: return ORANGE;
            case 3: return YELLOW;
            case 4: return GREEN;
            case 5: return BLUE;
            case 6: return INDIGO;
            case 7: return VIOLET;
            case 8: return CYAN;
            case 9: return WHITE;
            default: return null;
        }
    } 
}
