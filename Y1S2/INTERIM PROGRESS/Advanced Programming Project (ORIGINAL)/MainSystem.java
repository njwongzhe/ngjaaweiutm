import java.util.Scanner;

public class MainSystem {
    public static void main(String [] args) {
        boolean startProgram = true;
        Scanner read = new Scanner(System.in);
        do{
            Menu menu = Menu.readAccessType(read);
            startProgram = menu.readMenuOption(read);
        } while(startProgram);
        
        read.close();
    }
}

//FIX
//OrderID order by dateTime -done
//MenuItemID -done
//receipt should print date -done
//Should check whether the phone is duplicate for customer 
//Admin account is based on username not combination -done
//Avoid nullPointerException, all the read infomation should be checked to not be null -done
//Need to load and save every method involving that ArrayList -done
//Customer Orders will be generated multiple times due to this.order = new Order(customer); if exiting and close CLI simultaneously. -done
//Customer, if cross order by different customer, order will be a mess -done
//Filter orders by date: should filter out those order unpaid and empty order items -done
//Admin, if an item is removed, an item with the same name cannot be added again, unless restart the program
//Admin, if an item is removed, its menuItemID will permanently removed, although a new item is added, the removed menuItemID will be skipped instead of reassigning to a new item


//ADD FUNCTION
//Description of FOOD and DRINK -done
//Get popular item -done
//Filter order by date -done
//Daily Summary Report -done
//Search Customer historical order