int main() {
int a,b,c,d,ADD1,ADD2,ADD3,SUB1,SUB2,SUB3,AND,OR,XOR;
a=5;
b=-10;
c=1;
d=-2;
ADD1=a+c;  //ADD with both operands +ve
ADD2=a+b;  //ADD with operands opposite sign
ADD3=b+d;  //ADD with both operands -ve
SUB1=a-c;  //SUB with both operands +ve
SUB2=a-b;  //SUB with both operands opposite sign
SUB3=b-d;  //SUB with both operands -ve
AND=a&c;   //AND operation
OR=a|c;    //OR operation
XOR=a^c;   //XOR operation
return 0;
}
