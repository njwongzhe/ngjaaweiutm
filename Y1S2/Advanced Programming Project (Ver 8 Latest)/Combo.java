import java.util.ArrayList;
import java.util.Scanner;

public class Combo extends MenuItem {
    private ArrayList<MenuItem> items;
    private ArrayList<Integer> quantities;
    private ArrayList<String> descriptionLines;
    private ArrayList<Boolean> exchangeable;
    private ArrayList<ArrayList<MenuItem>> exchangeList;
    private ArrayList<ArrayList<Double>> exchangeFees;

    public Combo() {
        super("C0"); // Temporary ID for loading
        this.items = new ArrayList<>();
        this.quantities = new ArrayList<>();
        this.descriptionLines = new ArrayList<>();
        this.exchangeable = new ArrayList<>();
        this.exchangeList = new ArrayList<>();
        this.exchangeFees = new ArrayList<>();
    }

    public Combo(String menuItemID) {
        super(menuItemID);
        this.items = new ArrayList<>();
        this.quantities = new ArrayList<>();
        this.descriptionLines = new ArrayList<>();
        this.exchangeable = new ArrayList<>();
        this.exchangeList = new ArrayList<>();
        this.exchangeFees = new ArrayList<>();
        allMenuItems.add(this);
    }

    @Override
    public String toCSVString() {
        StringBuilder sb = new StringBuilder();
        // Format: "id1:qty1:exch:exchItems|id2:qty2:exch,..."
        for (int i = 0; i < items.size(); i++) {
            sb.append(items.get(i).getMenuItemID())
              .append(":")
              .append(quantities.get(i))
              .append(":")
              .append(exchangeable.get(i));
            
            if (exchangeable.get(i) && !exchangeList.get(i).isEmpty()) {
                sb.append(":");
                // Format exchange items as itemID-fee|itemID-fee
                for (int j = 0; j < exchangeList.get(i).size(); j++) {
                    if (j > 0) sb.append("|");
                    sb.append(exchangeList.get(i).get(j).getMenuItemID())
                      .append("-")
                      .append(exchangeFees.get(i).get(j));
                }
            }
            sb.append(";");  // Use semicolon between items
        }

        ArrayList<String> escapedDescLines = new ArrayList<>();
        for (String line : descriptionLines) {
            escapedDescLines.add(escapeComma(escapeColon(line)));
        }
        
        return String.format("Combo,%s,%.2f,%s,%s,%s,%b",
            escapeComma(name),
            price,
            menuItemID,
            escapeComma(String.join("||", escapedDescLines)),  // Use double pipe for description
            sb.toString(),
            available);
    }

    @Override
    public void printDescription(boolean isAdmin) {
        super.printAvailable(isAdmin);
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
        
        // Split description lines
        String[] descLines = unescapeComma(parts[4]).split("\\|\\|");
        for (String line : descLines) {
            combo.descriptionLines.add(unescapeColon(line));
        }
        
        // Split combo items
        String[] itemEntries = parts[5].split(";");
        for (String entry : itemEntries) {
            if (!entry.isEmpty()) {
                String[] components = entry.split(":");
                if (components.length >= 3) {
                    String itemId = components[0];
                    int qty = Integer.parseInt(components[1]);
                    boolean exch = Boolean.parseBoolean(components[2]);
                    
                    MenuItem foundItem = findItemById(allItems, itemId);
                    if (foundItem != null) {
                        combo.items.add(foundItem);
                        combo.quantities.add(qty);
                        combo.exchangeable.add(exch);
                        
                        ArrayList<MenuItem> exchItems = new ArrayList<>();
                        ArrayList<Double> exchFees = new ArrayList<>();
                        
                        if (exch && components.length > 3) {
                            String[] exchParts = components[3].split("\\|");
                            for (String exchPart : exchParts) {
                                String[] itemFee = exchPart.split("-");
                                if (itemFee.length == 2) {
                                    String exchItemId = itemFee[0];
                                    double fee = Double.parseDouble(itemFee[1]);
                                    MenuItem exchItem = findItemById(allItems, exchItemId);
                                    if (exchItem != null) {
                                        exchItems.add(exchItem);
                                        exchFees.add(fee);
                                    }
                                }
                            }
                        }
                        
                        combo.exchangeList.add(exchItems);
                        combo.exchangeFees.add(exchFees);
                    }
                }
            }
        }
        return combo;
    }

    private static MenuItem findItemById(ArrayList<MenuItem> items, String id) {
        for (MenuItem item : items) {
            if (item.getMenuItemID().equals(id)) {
                return item;
            }
        }
        return null;
    }

    public static Combo readComboDetails(Scanner read, ArrayList<MenuItem> allItems) {
        Combo combo = new Combo(generateNewID("Combo"));
        
        readCommonDetails(combo, read);
        
        while (true) {
            // Display available items excluding other combos
            System.out.println("\nAvailable Items:");
            for (MenuItem item : allItems) {
                if (!(item instanceof Combo)) {
                    System.out.printf("%s. %s (RM%.2f)\n", 
                        item.getMenuItemID(), item.getName(), item.getPrice());
                }
            }
            
            System.out.printf(ColourManager.ouColour() + "\nEnter MenuItem ID or Name to Add to the Combo (OR 'done' to Finish): " + ColourManager.reColour());
            System.out.print(ColourManager.inColour());
            String input = read.nextLine().trim();
            System.out.print(ColourManager.reColour());
            
            if (input.toLowerCase().equals("done")) {
                if (combo.items.isEmpty()) {
                    System.out.println(ColourManager.erColour() + "Combo must contain at least one item!" + ColourManager.reColour());
                    continue;
                }
                break;
            }
            
            // Find item by ID or name
            MenuItem selectedItem = null;
            for (MenuItem item : allItems) {
                if (!(item instanceof Combo) && 
                    (item.getMenuItemID().toLowerCase().equals(input.toLowerCase()) || 
                    item.getName().toLowerCase().equals(input.toLowerCase()))) {
                    selectedItem = item;
                    break;
                }
            }
            
            if (selectedItem == null) {
                System.out.println(ColourManager.erColour() + "Invalid item! Please try again." + ColourManager.reColour());
                continue;
            }
            
            // Get quantity
            int quantity = 0;
            while (quantity <= 0) {
                System.out.printf(ColourManager.ouColour() + "Enter Quantity for %s: ", selectedItem.getName());
                System.out.print(ColourManager.reColour());
                try {
                    System.out.print(ColourManager.inColour());
                    quantity = Integer.parseInt(read.nextLine());
                    System.out.print(ColourManager.reColour());
                    if (quantity <= 0) {
                        System.out.println(ColourManager.erColour() + "Quantity must be positive!" + ColourManager.reColour());
                    }
                } catch (NumberFormatException e) {
                    System.out.println(ColourManager.erColour() + "Please enter a valid number!" + ColourManager.reColour());
                }
            }
            
            // Ask if this item is exchangeable
            boolean exchangeable = false;
            ArrayList<MenuItem> exchangeItems = new ArrayList<>();
            ArrayList<Double> exchangeFees = new ArrayList<>();
            
            System.out.printf(ColourManager.ouColour() + "Can customers exchange this item in the combo? (y/n): " + ColourManager.reColour());
            System.out.print(ColourManager.inColour());
            String exchInput = read.nextLine().trim().toLowerCase();
            System.out.print(ColourManager.reColour());
            
            if (exchInput.toLowerCase().equals("y")) {
                exchangeable = true;
                
                // Ask if admin wants to see available items
                System.out.printf(ColourManager.ouColour() + "Show available items for exchange? (y/n): " + ColourManager.reColour());
                System.out.print(ColourManager.inColour());
                String showItems = read.nextLine().trim().toLowerCase();
                System.out.print(ColourManager.reColour());
                
                if (showItems.toLowerCase().equals("y")) {
                    System.out.println("\nAvailable items for exchange (same type):");
                    ArrayList<MenuItem> sameTypeItems = new ArrayList<>();
                    
                    for (MenuItem item : allItems) {
                        if (!(item instanceof Combo) && 
                            ((selectedItem instanceof Food && item instanceof Food) || 
                            (selectedItem instanceof Drink && item instanceof Drink))) {
                            System.out.printf("%s. %s (RM%.2f)\n", item.getMenuItemID(), item.getName(), item.getPrice());
                            sameTypeItems.add(item);
                        }
                    }
                }
                
                // Add exchange items one by one
                while (true) {
                    System.out.printf(ColourManager.ouColour() + "\nEnter item ID or name to add to exchange list (or 'done' to finish): " + ColourManager.reColour());
                    System.out.print(ColourManager.inColour());
                    String exchItemInput = read.nextLine().trim();
                    System.out.print(ColourManager.reColour());
                    
                    if (exchItemInput.toLowerCase().equals("done")) {
                        break;
                    }
                    
                    // Find exchange item by ID or name
                    MenuItem exchItem = null;
                    for (MenuItem item : allItems) {
                        if (!(item instanceof Combo) && 
                            ((selectedItem instanceof Food && item instanceof Food) || 
                            (selectedItem instanceof Drink && item instanceof Drink)) &&
                            (item.getMenuItemID().toLowerCase().equals(exchItemInput.toLowerCase()) || 
                            item.getName().toLowerCase().equals(exchItemInput.toLowerCase()))) {
                            exchItem = item;
                            break;
                        }
                    }
                    
                    if (exchItem == null) {
                        System.out.println(ColourManager.erColour() + "Invalid item or not same type! Please try again." + ColourManager.reColour());
                        continue;
                    }
                    
                    if (exchItem.equals(selectedItem)) {
                        System.out.println(ColourManager.erColour() + "Cannot exchange for the same item!" + ColourManager.reColour());
                        continue;
                    }
                    
                    // Get extra fee
                    double fee = 0;
                    boolean validFee = false;
                    while (!validFee) {
                        System.out.printf(ColourManager.ouColour() + "Enter Extra Fee for Exchanging to %s (RM): ", exchItem.getName());
                        System.out.print(ColourManager.reColour());
                        try {
                            System.out.print(ColourManager.inColour());
                            fee = Double.parseDouble(read.nextLine());
                            System.out.print(ColourManager.reColour());
                            if (fee >= 0) {
                                validFee = true;
                            } else if (fee < 0) {
                                System.out.println(ColourManager.erColour() + "Please enter a positive number.\n" + ColourManager.reColour());
                            }
                        } catch (NumberFormatException e) {
                            System.out.println(ColourManager.erColour() + "Invalid fee amount. Please enter a number.\n" + ColourManager.reColour());
                        }
                    }
                    
                    exchangeItems.add(exchItem);
                    exchangeFees.add(fee);
                    System.out.printf(ColourManager.suColour() + "Added %s to exchange list with RM%.2f extra fee.\n", exchItem.getName(), fee);
                    System.out.print(ColourManager.reColour());
                }
            }
            
            // Add the item to combo
            combo.items.add(selectedItem);
            combo.quantities.add(quantity);
            combo.exchangeable.add(exchangeable);
            combo.exchangeList.add(exchangeItems);
            combo.exchangeFees.add(exchangeFees);
            combo.descriptionLines.add(quantity + " x " + selectedItem.getName().toUpperCase());
            
            System.out.printf(ColourManager.suColour() + "Added %d x %s to combo.\n", quantity, selectedItem.getName());
            System.out.print(ColourManager.reColour());
        }
        
        return combo;
    }

    private static String escapeColon(String input) {
        if (input == null) return "";
        return input.replace(":", "\\:");
    }

    private static String unescapeColon(String input) {
        if (input == null) return "";
        return input.replace("\\:", ":");
    }

    public ArrayList<MenuItem> getItems() {
        return items;
    }

    public ArrayList<Integer> getQuantities() {
        return quantities;
    }

    public ArrayList<String> getDescriptionLines() {
        return descriptionLines;
    }

    public ArrayList<Boolean> getExchangeable() {
        return exchangeable;
    }

    public ArrayList<ArrayList<MenuItem>> getExchangeList() {
        return exchangeList;
    }
    
    public ArrayList<ArrayList<Double>> getExchangeFees() {
        return exchangeFees;
    }
}


