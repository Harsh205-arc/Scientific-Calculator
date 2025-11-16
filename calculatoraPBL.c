#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#define MAX_STACK 100
#define MAX_SIZE 10

/* ---------------- STACK CALCULATOR SECTION ---------------- */

int top = -1;
double val[MAX_STACK];

void pushVal(double a) {
    if (top >= MAX_STACK - 1) {
        printf("Error: Value stack full\n");
        exit(1);
    }
    val[++top] = a;
}

double pop_val() {
    if (top == -1) {
        printf("Error: Value stack is empty\n");
        exit(1);
    }
    return val[top--];
}

char op[MAX_STACK];
int topOp = -1;

void pushOp(char a) {
    if (topOp >= MAX_STACK - 1) {
        printf("Error: Operator stack full\n");
        exit(1);
    }
    op[++topOp] = a;
}

char popOp() {
    if (topOp == -1) {
        printf("Error: Operator stack empty\n");
        exit(1);
    }
    return op[topOp--];
}

char peek() {
    return op[topOp];
}

int precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    if (op == '^') return 3;
    if (isalpha(op)) return 4;
    return 0;
}

void applOp() {
    double deg_to_rad = M_PI / 180.0;
    double rad_to_deg = 180.0 / M_PI;
    char o = popOp();
    double val2, val1, result;

    // Handle single operand functions
    if (strchr("sctleSCTA", o)) {
        val2 = pop_val();
        switch (o) {
            case 's': result = sin(val2 * deg_to_rad); break;
            case 'c': result = cos(val2 * deg_to_rad); break;
            case 't': result = tan(val2 * deg_to_rad); break;
            case 'l': result = log(val2); break;
            case 'e': result = exp(val2); break;
            case 'S': result = asin(val2) * rad_to_deg; break;
            case 'C': result = acos(val2) * rad_to_deg; break;
            case 'T': result = atan(val2) * rad_to_deg; break;
        }
        pushVal(result);
        return;
    }

    // Binary operations
    val2 = pop_val();
    val1 = pop_val();
    switch (o) {
        case '+': result = val1 + val2; break;
        case '-': result = val1 - val2; break;
        case '*': result = val1 * val2; break;
        case '/':
            if (val2 == 0) {
                printf("Error: Division by zero\n");
                exit(1);
            }
            result = val1 / val2;
            break;
        case '^': result = pow(val1, val2); break;
        default:
            printf("Error: Unknown operator %c\n", o);
            exit(1);
    }
    pushVal(result);
}

void runCalculator() {
    top = topOp = -1;
    char line[256];
    printf("\nEnter your expression: ");
    getchar(); // clear buffer
    fgets(line, sizeof(line), stdin);

    char *ptr = line;
    char *endPtr;

    while (*ptr != '\0' && *ptr != '\n') {
        if (isspace(*ptr)) { ptr++; continue; }

        if (isalpha(*ptr)) {
            char func[10];
            int i = 0;
            while (isalpha(*ptr) && i < 9) func[i++] = *ptr++;
            func[i] = '\0';

            if (strcmp(func, "sin") == 0) pushOp('s');
            else if (strcmp(func, "cos") == 0) pushOp('c');
            else if (strcmp(func, "tan") == 0) pushOp('t');
            else if (strcmp(func, "log") == 0) pushOp('l');
            else if (strcmp(func, "exp") == 0) pushOp('e');
            else if (strcmp(func, "asin") == 0) pushOp('S');
            else if (strcmp(func, "acos") == 0) pushOp('C');
            else if (strcmp(func, "atan") == 0) pushOp('T');
            else {
                printf("Error: Unknown function '%s'\n", func);
                return;
            }
            continue;
        }

        if (isdigit(*ptr) || (*ptr == '.' && isdigit(ptr[1])) || (*ptr == '-' && (ptr == line || ptr[-1] == '(' || precedence(ptr[-1]) > 0))) {
            double value = strtod(ptr, &endPtr);
            pushVal(value);
            ptr = endPtr;
            continue;
        }

        if (*ptr == '(') { pushOp(*ptr); ptr++; continue; }

        if (*ptr == ')') {
            while (topOp >= 0 && peek() != '(') applOp();
            if (topOp == -1) { printf("Error: Mismatched parentheses\n"); return; }
            popOp();
            if (topOp >= 0 && isalpha(peek())) applOp();
            ptr++;
            continue;
        }

        if (*ptr == '+' || *ptr == '-' || *ptr == '*' || *ptr == '/' || *ptr == '^') {
            while (topOp >= 0 && (precedence(peek()) >= precedence(*ptr)) && peek() != '(') applOp();
            pushOp(*ptr);
            ptr++;
            continue;
        }

        printf("Error: Unknown character '%c'\n", *ptr);
        return;
    }

    while (topOp >= 0) {
        if (peek() == '(') { printf("Error: Mismatched parentheses\n"); return; }
        applOp();
    }

    printf("Result: %g\n", pop_val());
}

/* ---------------- POLYNOMIAL ADDITION SECTION ---------------- */

struct Node {
    int coeff;
    int pow;
    struct Node* next;
};

struct Node* createNode(int coeff, int pow) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    newNode->coeff = coeff;
    newNode->pow = pow;
    newNode->next = NULL;
    return newNode;
}

void insertNode(struct Node** head, int coeff, int pow) {
    struct Node* newNode = createNode(coeff, pow);
    if (*head == NULL)
        *head = newNode;
    else {
        struct Node* temp = *head;
        while (temp->next != NULL)
            temp = temp->next;
        temp->next = newNode;
    }
}

struct Node* addPolynomials(struct Node* p1, struct Node* p2) {
    struct Node* result = NULL;
    while (p1 != NULL && p2 != NULL) {
        if (p1->pow == p2->pow) {
            insertNode(&result, p1->coeff + p2->coeff, p1->pow);
            p1 = p1->next;
            p2 = p2->next;
        } else if (p1->pow > p2->pow) {
            insertNode(&result, p1->coeff, p1->pow);
            p1 = p1->next;
        } else {
            insertNode(&result, p2->coeff, p2->pow);
            p2 = p2->next;
        }
    }
    while (p1 != NULL) { insertNode(&result, p1->coeff, p1->pow); p1 = p1->next; }
    while (p2 != NULL) { insertNode(&result, p2->coeff, p2->pow); p2 = p2->next; }
    return result;
}

void display(struct Node* head) {
    while (head != NULL) {
        printf("%dx^%d", head->coeff, head->pow);
        head = head->next;
        if (head != NULL) printf(" + ");
    }
    printf("\n");
}

void runPolynomialAddition() {
    struct Node *poly1 = NULL, *poly2 = NULL, *sum = NULL;
    int n, coeff, pow;

    printf("\nEnter number of terms in 1st polynomial: ");
    scanf("%d", &n);
    printf("Enter coefficient and power (in descending order):\n");
    for (int i = 0; i < n; i++) {
        scanf("%d %d", &coeff, &pow);
        insertNode(&poly1, coeff, pow);
    }

    printf("Enter number of terms in 2nd polynomial: ");
    scanf("%d", &n);
    printf("Enter coefficient and power (in descending order):\n");
    for (int i = 0; i < n; i++) {
        scanf("%d %d", &coeff, &pow);
        insertNode(&poly2, coeff, pow);
    }

    sum = addPolynomials(poly1, poly2);

    printf("\nFirst Polynomial: ");
    display(poly1);
    printf("Second Polynomial: ");
    display(poly2);
    printf("Sum of Polynomials: ");
    display(sum);
}

/* ---------------- MATRIX OPERATIONS SECTION ---------------- */

#define MAX 10

void inputMatrix(int a[MAX][MAX], int r, int c) {
    printf("Enter elements (%dx%d):\n", r, c);
    for (int i = 0; i < r; i++)
        for (int j = 0; j < c; j++)
            scanf("%d", &a[i][j]);
}

void displayMatrix(int a[MAX][MAX], int r, int c) {
    for (int i = 0; i < r; i++) {
        for (int j = 0; j < c; j++)
            printf("%d ", a[i][j]);
        printf("\n");
    }
}

void addMatrix(int a[MAX][MAX], int b[MAX][MAX], int r, int c) {
    int res[MAX][MAX];
    for (int i = 0; i < r; i++)
        for (int j = 0; j < c; j++)
            res[i][j] = a[i][j] + b[i][j];
    printf("\nResult (A + B):\n");
    displayMatrix(res, r, c);
}

void subtractMatrix(int a[MAX][MAX], int b[MAX][MAX], int r, int c) {
    int res[MAX][MAX];
    for (int i = 0; i < r; i++)
        for (int j = 0; j < c; j++)
            res[i][j] = a[i][j] - b[i][j];
    printf("\nResult (A - B):\n");
    displayMatrix(res, r, c);
}

void multiplyMatrix(int a[MAX][MAX], int b[MAX][MAX], int r1, int c1, int r2, int c2) {
    if (c1 != r2) {
        printf("Matrix multiplication not possible (cols of A != rows of B)\n");
        return;
    }
    int res[MAX][MAX] = {0};
    for (int i = 0; i < r1; i++)
        for (int j = 0; j < c2; j++)
            for (int k = 0; k < c1; k++)
                res[i][j] += a[i][k] * b[k][j];
    printf("\nResult (A x B):\n");
    displayMatrix(res, r1, c2);
}

void transposeMatrix(int a[MAX][MAX], int r, int c) {
    printf("\nTranspose:\n");
    for (int i = 0; i < c; i++) {
        for (int j = 0; j < r; j++)
            printf("%d ", a[j][i]);
        printf("\n");
    }
}

void runMatrixModule() {
    int choice, r1, c1, r2, c2;
    int A[MAX][MAX], B[MAX][MAX];

    do {
        printf("\n------ MATRIX OPERATIONS ------\n");
        printf("1. Addition\n2. Subtraction\n3. Multiplication\n4. Transpose\n5. Back to Main Menu\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter rows and columns: ");
                scanf("%d %d", &r1, &c1);
                inputMatrix(A, r1, c1);
                inputMatrix(B, r1, c1);
                addMatrix(A, B, r1, c1);
                break;
            case 2:
                printf("Enter rows and columns: ");
                scanf("%d %d", &r1, &c1);
                inputMatrix(A, r1, c1);
                inputMatrix(B, r1, c1);
                subtractMatrix(A, B, r1, c1);
                break;
            case 3:
                printf("Enter rows and columns of Matrix A: ");
                scanf("%d %d", &r1, &c1);
                printf("Enter rows and columns of Matrix B: ");
                scanf("%d %d", &r2, &c2);
                inputMatrix(A, r1, c1);
                inputMatrix(B, r2, c2);
                multiplyMatrix(A, B, r1, c1, r2, c2);
                break;
            case 4:
                printf("Enter rows and columns: ");
                scanf("%d %d", &r1, &c1);
                inputMatrix(A, r1, c1);
                transposeMatrix(A, r1, c1);
                break;
            case 5:
                printf("Returning to Main Menu...\n");
                break;
            default:
                printf("Invalid choice!\n");
        }
    } while (choice != 5);
}

/* ---------------- MAIN MENU ---------------- */

int main() {
    int choice;
    do {
        printf("\n========== MATH LAB ==========\n");
        printf("1. Stack Calculator\n");
        printf("2. Polynomial Addition\n");
        printf("3. Matrix Operations\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: runCalculator(); break;
            case 2: runPolynomialAddition(); break;
            case 3: runMatrixModule(); break;
            case 4: printf("Exiting program...\n"); break;
            default: printf("Invalid choice! Try again.\n");
        }
    } while (choice != 4);

    return 0;
}
