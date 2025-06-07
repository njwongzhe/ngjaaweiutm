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
            System.out.printf("\nEnter MenuItem ID or Name (or 'done' to finish): ");
            String input = read.nextLine().trim();
            
            if (input.toLowerCase().equals("done")) {
                if (combo.items.isEmpty()) {
                    System.out.println("Combo must contain at least one item!");
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
                System.out.println("Invalid item! Please try again.");
                continue;
            }
            
            int quantity = 0;
            while (quantity <= 0) {
                System.out.printf("Enter quantity for %s: ", selectedItem.getName());
                try {
                    quantity = Integer.parseInt(read.nextLine());
                    if (quantity <= 0) {
                        System.out.println("Quantity must be positive!");
                    }
                } catch (NumberFormatException e) {
                    System.out.println("Please enter a valid number!");
                }
            }
            
            combo.items.add(selectedItem);
            combo.quantities.add(quantity);
            combo.descriptionLines.add(quantity + " x " + selectedItem.getName().toUpperCase());
            
            System.out.printf("Added %d x %s to combo\n", quantity, selectedItem.getName());
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