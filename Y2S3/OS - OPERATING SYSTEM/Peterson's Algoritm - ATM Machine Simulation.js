let accountBalance = 1000.00;        // Shared resource (Critical Data).

let flag = [false, false];           // PETERSON: Indicate if a process is ready to enter the critical section.
let turn = 0;                        // PETERSON: Indicate whose turn to enter the critical section.

let transactionHistory = [];         // Transaction log.
let cancelRequest = [false, false];  // Cancellation flags.
const totalDurationMs = 3000;        // Transaction duration.

// ========================================================================
// TRANSACTION HISTORY FUNCTIONS
// ========================================================================

/**
 * Add item to transaction history.
 * @param {number} userId - ID of user (0 or 1).
 * @param {string} type - Type of transaction ('deposit' or 'withdraw').
 * @param {number} amount - Amount of money involved in the transaction.
 * @param {number} balanceBefore - Account balance before the transaction.
 * @param {number} balanceAfter - Account balance after the transaction.
 */
function addToHistory(userId, type, amount, balanceBefore, balanceAfter) {
    const now = new Date();
    const timeString = now.toLocaleTimeString();

    const historyItem = {
        time: timeString,
        user: userId,
        type: type,
        amount: amount,
        balanceBefore: balanceBefore,
        balanceAfter: balanceAfter
    };

    transactionHistory.unshift(historyItem); // Add to beginning of transactionHistory array.
    updateHistoryDisplay();                  // Update the screen (Transaction History Part).
}

/**
 * Allow the transaction history list in the UI.
 */
function updateHistoryDisplay() {
    const historyList = document.getElementById('historyList');

    // Shows a 'No Transactions Yet' message if history in array transactionHistory is empty.
    if (transactionHistory.length === 0) {
        historyList.innerHTML = '<div class="noHistory">No Transactions Yet</div>';
        return;
    }

    historyList.innerHTML = transactionHistory.map(item => `
        <div class="historyItem ${item.type}">
            <div class="time">${item.time}</div>
            <div class="details">User ${item.user} - ${item.type.toUpperCase()}</div>
            <div class="amount">${item.type === 'deposit' ? '+' : '-'} RM ${item.amount.toFixed(2)}</div>
            <div class="time">Balance: RM ${item.balanceAfter.toFixed(2)}</div>
        </div>
    `).join('');
}

/**
 * Initializes event listeners for the transaction history list.
 * Enables horizontal scrolling using the mouse wheel when the
 * history list overflows horizontally. Prevents vertical page
 * scrolling when hovering over the history area.
 */
function initHistoryListEvents() {
    const historyList = document.getElementById('historyList');

    if(historyList) {
        // When user scrolls the mouse wheel over the history area.
        historyList.addEventListener('wheel', function (e) {
            if (historyList.scrollWidth > historyList.clientWidth) {
                e.preventDefault();
                historyList.scrollLeft += e.deltaY + e.deltaX;
            }
        }, { passive: false });

        // Prevent wheel propagation to the page when over the history area.
        historyList.addEventListener('wheel', function (e) {
            if (historyList.scrollWidth > historyList.clientWidth) {
                e.stopPropagation();
            }
        }, { passive: false });
    }
}

// ========================================================================
// ATM TRANSACTION FUNCTIONS
// ========================================================================

/**
 * Updates the status message displayed for a specific user.
 * @param {number} userId - The ID of the user (0 or 1).
 * @param {string} msg - The status message to display ('Depositing...', 'Waiting', 'Completed', etc).
 */
function setStatus(userId, msg) {
    const el = document.getElementById(`status${userId}`); // id 'status0' or 'status1'.
    if(el) el.textContent = msg;                           // Only add the message if the element exists.
}

/**
 * Cancels a waiting or error state for a specific user.
 * @param {number} userId - The ID of the user (0 or 1) to cancel.
 */
function cancelWait(userId) {
    cancelRequest[userId] = true;

    const panel = document.getElementById(`user${userId}`);
    const progressFill = document.getElementById(`progress${userId}`);
    const btn = document.getElementById(`btn${userId}`);
    const cancelBtn = document.getElementById(`cancel${userId}`);

    // If not actively running, clear error/wait state immediately.
    if (!panel.classList.contains('active') && !panel.classList.contains('waiting')) {
        panel.classList.remove('error');
        setStatus(userId, '');
        progressFill.style.width = '0%';
        progressFill.style.background = '#27ae60';
        btn.disabled = false;
        cancelBtn.disabled = true;
    }
}

/**
 * Starts a deposit or withdrawal transaction using Peterson's Algorithm.
 * Implements mutual exclusion to ensure only one user can access the shared account balance at a time.
 * Includes entry section (with busy-wait), critical section (balance modification) and exit section.
 * @async
 * @param {number} userId - The ID of the user initiating the transaction (0 or 1).
 * @param {string} type - The type of transaction ('deposit' or 'withdraw').
 * @returns {Promise<void>} Resolves when the transaction completes or is cancelled.
 */
async function startTransaction(userId, type) {
    const otherId = 1 - userId;
    const panel = document.getElementById(`user${userId}`);
    const progressFill = document.getElementById(`progress${userId}`);
    const amount = parseFloat(document.getElementById(`amount${userId}`).value);
    const btn = document.getElementById(`btn${userId}`);
    const cancelBtn = document.getElementById(`cancel${userId}`);

    // Validate amount.
    if(isNaN(amount) || amount <= 0) {
        setStatus(userId, 'Enter an Amount > 0');
        panel.classList.add('error');
        progressFill.style.background = '#e74c3c';
        progressFill.style.width = '100%';
        cancelBtn.disabled = false;
        return;
    }

    // Prevent start if insufficient funds (withdraw).
    if(type === 'withdraw' && amount > accountBalance) {
        setStatus(userId, 'Insufficient Funds');
        panel.classList.add('error');
        progressFill.style.background = '#e74c3c';
        progressFill.style.width = '100%';
        cancelBtn.disabled = false;
        return;
    }

    // ========================================================================
    // PETERSON'S ALGORITHM - ENTRY SECTION
    // ========================================================================
    flag[userId] = true; // PETERSON: 'userId' wants to enter critical section.
    turn = otherId;      // PETERSON: Give turn to other process. (otherId = 1 - userId)

    btn.disabled = true;
    cancelBtn.disabled = true;
    cancelRequest[userId] = false;
    panel.classList.remove('error');

    // PETERSON: Busy-wait Condition - Wait while other process wants in AND it's their turn.
    if(flag[otherId] && turn === otherId) {
        // Change the UI to show waiting state.
        panel.classList.add('waiting');
        progressFill.style.width = '100%';
        progressFill.style.background = '#e67e22';
        setStatus(userId, `Waiting: User ${otherId} in Progress`);
        cancelBtn.disabled = false;
        
        // PETERSON: Busy-wait Loop
        while(flag[otherId] && turn === otherId) {
            // User requested cancellation when waiting.
            if(cancelRequest[userId]) {
                flag[userId] = false;
                panel.classList.remove('waiting');
                btn.disabled = false;
                cancelBtn.disabled = true;
                progressFill.style.width = '0%';
                progressFill.style.background = '#27ae60';
                setStatus(userId, 'Cancelled');
                cancelRequest[userId] = false;
                return;
            }

            // User is still waiting.
            await new Promise(r => setTimeout(r, 100));
        }

        // Exiting waiting state.
        panel.classList.remove('waiting');
        cancelBtn.disabled = true;
        setStatus(userId, '');
    }

    // ========================================================================
    // PETERSON'S ALGORITHM - CRITICAL SECTION
    // ========================================================================

    // Change the UI to show active transaction state.
    panel.classList.add('active');
    progressFill.style.background = '#27ae60';
    progressFill.style.width = '100%';
    setStatus(userId, type === 'deposit' ? 'Depositing...' : 'Withdrawing...');

    // Countdown logic.
    let timeLeftMs = totalDurationMs;
    const interval = 100;
    while(timeLeftMs > 0) {
        await new Promise(r => setTimeout(r, interval));
        timeLeftMs -= interval;
        const pct = Math.max(0, (timeLeftMs / totalDurationMs) * 100);
        progressFill.style.width = `${pct}%`;
    }
    progressFill.style.width = '0%';

    // CRITICAL SECTION: Accessing shared resource (accountBalance).
    // CRITICAL Operation: Modifying the shared bank balance.
    const balanceBefore = accountBalance; // Store balance before transaction for history logging.
    let success = false;                  // Track if transaction succeeded.
    if(type === 'deposit') {
        accountBalance += amount; // CRITICAL: Modify shared resource.
        addToHistory(userId, 'deposit', amount, balanceBefore, accountBalance);
        success = true;
    } 
    else { // (type === 'withdraw')
        accountBalance -= amount; // CRITICAL: Modify shared resource.
        addToHistory(userId, 'withdraw', amount, balanceBefore, accountBalance);
        success = true;
    }

    // Update Account Balance Display.
    document.getElementById('mainBalance').innerText = accountBalance.toFixed(2);

    if(success) {
        panel.classList.remove('error');
        setStatus(userId, 'Completed');
    }

    // ========================================================================
    // PETERSON'S ALGORITHM - EXIT SECTION
    // ========================================================================
    flag[userId] = false; // PETERSON: Signal exit from critical section.
    
    // ========================================================================
    // PETERSON'S ALGORITHM - REMAINDER SECTION
    // ========================================================================
    panel.classList.remove('active');
    btn.disabled = false;
    progressFill.style.width = '0%';
    cancelBtn.disabled = true;
}

// Initialize when DOM is ready.
// Allow horizontal scrolling of transaction history list after DOM content is loaded.
document.addEventListener('DOMContentLoaded', initHistoryListEvents);