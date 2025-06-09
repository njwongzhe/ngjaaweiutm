import java.util.ArrayList;
import java.util.Objects;
import java.util.Scanner;

public abstract class MenuItem {
    protected String name;
    protected double price;
    protected String menuItemID;
    protected static ArrayList<MenuItem> allMenuItems = new ArrayList<MenuItem>();

    private static int foodCounter = 0;
    private static int drinkCounter = 0;
    private static int comboCounter = 0;
    private static ArrayList<String> availableFoodIDs = new ArrayList<>();
    private static ArrayList<String> availableDrinkIDs = new ArrayList<>();
    private static ArrayList<String> availableComboIDs = new ArrayList<>();

    public MenuItem(String menuItemID) {
        this.name = "Unknown";
        this.price = 0.0;
        this.menuItemID = menuItemID;
    }

    protected static String generateNewID(String type) {
        switch (type) {
            case "Food":
                if (!availableFoodIDs.isEmpty()) {
                    return availableFoodIDs.remove(0); // Use oldest recycled ID first
                }
                return "F" + (++foodCounter);
            case "Drink":
                if (!availableDrinkIDs.isEmpty()) {
                    return availableDrinkIDs.remove(0);
                }
                return "D" + (++drinkCounter);
            case "Combo":
                if (!availableComboIDs.isEmpty()) {
                    return availableComboIDs.remove(0);
                }
                return "C" + (++comboCounter);
            default:
                throw new IllegalArgumentException("Invalid menu item type");
        }
    }

    // Used to add recylceID when item is deleted
    public static void recycleMenuItemID(MenuItem item) {
        if (item == null) return;
        
        String id = item.getMenuItemID();
        if (id.startsWith("F")) {
            availableFoodIDs.add(id);
            availableFoodIDs.sort(null); // Keep IDs in order
        } else if (id.startsWith("D")) {
            availableDrinkIDs.add(id);
            availableDrinkIDs.sort(null);
        } else if (id.startsWith("C")) {
            availableComboIDs.add(id);
            availableComboIDs.sort(null);
        }
    }

    public static void initializeIDGenerator(ArrayList<MenuItem> items) {
        // Reset counters and clear recycled IDs
        foodCounter = 0;
        drinkCounter = 0;
        comboCounter = 0;
        availableFoodIDs.clear();
        availableDrinkIDs.clear();
        availableComboIDs.clear();

        // Track all existing IDs
        ArrayList<String> existingFoodIDs = new ArrayList<>();
        ArrayList<String> existingDrinkIDs = new ArrayList<>();
        ArrayList<String> existingComboIDs = new ArrayList<>();

        // Find maximum existing IDs and collect all IDs
        for (MenuItem item : items) {
            String id = item.menuItemID;
            if (id.startsWith("F")) {
                existingFoodIDs.add(id);
                int num = Integer.parseInt(id.substring(1));
                if (num > foodCounter) foodCounter = num;
            } else if (id.startsWith("D")) {
                existingDrinkIDs.add(id);
                int num = Integer.parseInt(id.substring(1));
                if (num > drinkCounter) drinkCounter = num;
            } else if (id.startsWith("C")) {
                existingComboIDs.add(id);
                int num = Integer.parseInt(id.substring(1));
                if (num > comboCounter) comboCounter = num;
            }
        }

        // Find potential recycled IDs
        findMissingIDs("F", foodCounter, existingFoodIDs, availableFoodIDs);
        findMissingIDs("D", drinkCounter, existingDrinkIDs, availableDrinkIDs);
        findMissingIDs("C", comboCounter, existingComboIDs, availableComboIDs);
    }

    private static void findMissingIDs(String prefix, int maxCount, ArrayList<String> existingIDs, ArrayList<String> availableIDs) {
        for (int i = 1; i <= maxCount; i++) {
            String id = prefix + i;
            if (!existingIDs.contains(id)) {
                availableIDs.add(id);
            }
        }
    }


    public void setName(String name) {
        this.name = name;
    }

    public String getName() {
        return this.name;
    }

    public void setPrice(double price) {
        this.price = price;
    }

    public double getPrice() {
        return this.price;
    }

    public String getMenuItemID() {
        return this.menuItemID;
    }

    protected static String escapeComma(String value) {
        return value.replace(",", "&#44;");
    }

    protected static String unescapeComma(String value) {
        return value.replace("&#44;", ",");
    }

    protected static int readValidNumber(Scanner read, int min, int max) {
        while (true) {
            try {
                int value = Integer.parseInt(read.nextLine());
                if (value >= min && value <= max) {
                    return value;
                }
                System.out.printf("Please Enter a Value Between %d and %d: ", min, max);
            } catch (NumberFormatException e) {
                System.out.printf("Invalid Input. Please Enter a Number: ");
            }
        }
    }

    protected static double readValidDouble(Scanner read, double min, double max) {
        while (true) {
            try {
                double value = Double.parseDouble(read.nextLine());
                if (value >= min && value <= max) {
                    return value;
                }
                System.out.printf("Please Enter a Value Between %.2f and %.2f: ", min, max);
            } catch (NumberFormatException e) {
                System.out.printf("Invalid Input. Please Enter a Number: ");
            }
        }
    }

    public static ArrayList<MenuItem> getAllMenuItems() {
        return allMenuItems;
    }

    protected static boolean isNameDuplicate(String name) {
        for (MenuItem item : allMenuItems) {
            if (item.getName().toLowerCase().equals(name.toLowerCase())) {
                return true;
            }
        }
        return false;
    }

    protected static boolean isValidItemName(String name) {
        // Check if name matches the pattern of menu item IDs F1, D2, C3 and so forth
        if (name.matches("^[FDCfdc]\\d+$")) {
            System.out.println("Error: Item name cannot be in the format of a menu item ID (F/D/C followed by numbers)");
            return false;
        }
        return true;
    }

    protected static void readCommonDetails(MenuItem item, Scanner read) {
        // Read name with duplicate check
        while (true) {
            System.out.printf("Enter Item Name: ");
            String name = read.nextLine().trim();
            
            if (name.isEmpty()) {
                System.out.println("Name cannot be empty!");
                continue;
            }

            if (!isValidItemName(name)) {
                continue;
            }
            
            if (isNameDuplicate(name)) {
                System.out.println("An item with this name already exists!");
                System.out.printf("\nDo You Want to Try a Different Name? (y/n): ");
                if (!read.nextLine().toLowerCase().equals("y")) {
                    throw new RuntimeException("Item creation cancelled due to duplicate name.\n");
                }
                continue;
            }
            
            item.setName(name);
            break;
        }
        
        // Read price
        System.out.printf("Enter Item Price: ");
        item.setPrice(readValidDouble(read, 0, Double.MAX_VALUE));
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj) return true;
        if (obj == null || getClass() != obj.getClass()) return false;
        MenuItem other = (MenuItem) obj;
        return this.menuItemID == other.menuItemID;
    }

    @Override
    public int hashCode() {
        return Objects.hash(menuItemID);
    }

    public abstract void printDescription();
    public abstract String toCSVString();
}
