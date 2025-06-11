import java.util.ArrayList;
import java.util.Scanner;

public class Combo extends MenuItem {
    private ArrayList<MenuItem> items;
    private ArrayList<Integer> quantities;
    private ArrayList<String> descriptionLines;

    public Combo() {
        super("C0"); // Temporary ID for loading
        this.items = new ArrayList<>();
        this.quantities = new ArrayList<>();
        this.descriptionLines = new ArrayList<>();
    }

    public Combo(String menuItemID) {
        super(menuItemID);
        this.items = new ArrayList<>();
        this.quantities = new ArrayList<>();
        this.descriptionLines = new ArrayList<>();
        allMenuItems.add(this);
    }

    @Override
    public String toCSVString() {
        StringBuilder sb = new StringBuilder();
        // Format: "id1:qty1,id2:qty2,..."
        for (int i = 0; i < items.size(); i++) {
            sb.append(items.get(i).getMenuItemID())
              .append(":")
              .append(quantities.get(i))
              .append(",");
        }
        return String.format("Combo,%s,%.2f,%s,%s,%s",
                escapeComma(name),
                price,
                menuItemID,
                escapeComma(String.join("|", descriptionLines)),
                sb.toString());
    }

    @Override
    public void printDescription() {
        System.out.println("Includes:");
        for (String line : descriptionLines) {
            System.out.println("- " + line);
        }
    }

    public static Combo fromCSVString(String[] parts, ArrayList<MenuItem> allItems) {
        if (parts.length < 6) return null;

        Combo combo = new Combo();
        combo.name = unescapeComma(parts[1]);
        combo.price = Double.parseDouble(parts[2]);
        combo.menuItemID = parts[3];
        
        String[] descLines = unescapeComma(parts[4]).split("\\|");
        for (String line : descLines) {
            combo.descriptionLines.add(line);
        }
        
        String[] itemEntries = parts[5].split(",");
        for (String entry : itemEntries) {
            if (!entry.isEmpty()) {
                String[] pair = entry.split(":");
                if (pair.length == 2) {
                    String itemId = pair[0];
                    int qty = Integer.parseInt(pair[1]);
                    
                    for (MenuItem item : allItems) {
                        if (item.getMenuItemID().equals(itemId)) {
                            combo.items.add(item);
                            combo.quantities.add(qty);
                            break;
                        }
                    }
                }
            }
        }
        
        return combo;
    }

    public static Combo readComboDetails(Scanner read, ArrayList<MenuItem> allItems) {
        Combo combo = new Combo(generateNewID("Combo"));
        
        readCommonDetails(combo, read);
        
        // Display available items excluding other combos
        System.out.println("\nAvailable Items:");
        for (MenuItem item : allItems) {
            if (!(item instanceof Combo)) {
                System.out.printf("%s. %s (RM%.2f)\n", 
                    item.getMenuItemID(), item.getName(), item.getPrice());
            }
        }
        
        while (true) {
            System.out.printf(ColourManager.ouColour() + "\nEnter MenuItem ID or Name (OR 'done' to Finish): " + ColourManager.reColour()); // Output
            System.out.print(ColourManager.inColour());
            String input = read.nextLine().trim(); // Input
            System.out.print(ColourManager.reColour());
            
            if (input.toLowerCase().equals("done")) {
                if (combo.items.isEmpty()) {
                    System.out.println(ColourManager.erColour() + "Combo must contain at least one item!" + ColourManager.reColour()); // Error
                    continue;
                }
                break;
            }
            
            MenuItem selectedItem = null;
            for (MenuItem item : allItems) {
                if (item.getMenuItemID().equals(input) && !(item instanceof Combo)) {
                    selectedItem = item;
                    break;
                }
            }
            
            if (selectedItem == null) {
                System.out.println(ColourManager.erColour() + "Invalid item! Please try again." + ColourManager.reColour()); // Error
                continue;
            }
            
            int quantity = 0;
            while (quantity <= 0) {
                System.out.printf(ColourManager.ouColour() + "Enter quantity for %s: ", selectedItem.getName()); // Ask Input
                System.out.print(ColourManager.reColour());
                try {
                    System.out.print(ColourManager.inColour());
                    quantity = Integer.parseInt(read.nextLine()); // Input
                    System.out.print(ColourManager.reColour());
                    if (quantity <= 0) {
                        System.out.println(ColourManager.erColour() + "Quantity must be positive!" + ColourManager.reColour()); // Error
                    }
                } catch (NumberFormatException e) {
                    System.out.println(ColourManager.erColour() + "Please enter a valid number!" + ColourManager.reColour()); // Error
                }
            }
            
            combo.items.add(selectedItem);
            combo.quantities.add(quantity);
            combo.descriptionLines.add(quantity + " x " + selectedItem.getName().toUpperCase());
            
            System.out.printf(ColourManager.suColour() + "Added %d x %s to combo.\n", quantity, selectedItem.getName()); // Success
            System.out.print(ColourManager.reColour());
        }
        
        return combo;
    }

    public ArrayList<MenuItem> getItems() {
        return items;
    }

    public ArrayList<Integer> getQuantities() {
        return quantities;
    }
}