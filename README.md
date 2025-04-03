Console-based C application that simulates a portion of the National Water
Commission (NWC) Utility Platform, specifically for Domestic Customers using the
Metric Metered system.

The application features two main user types: Agent and Customer.

Agent functionalities:
- Manage customer accounts (add, edit, view, delete/archive)
- Generate bills
- View reports


Customer functionalities:
- Register an account
- Register a payment card for bill payments
- View and pay bills
- Surrender meters

Checks are done to ensure each user has appropriate authorization to access the system.
All data is persisted in files so that records are maintained across program executions.
