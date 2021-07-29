#include <iostream>
#include <cmath>
#include <iomanip>

using namespace std;


int main()
{
    double x;
    cout << "Enter x\n";
    cin >> x;
    if (0 >= x || x > 2) {
        cout << "Error!\n";
        return 0;
    }
    
    double eps;
    cout << "Enter epsilon\n";
    cin >> eps;

    double x0 = x-1;
    double cresult = x-1;  
    int i = 0;

    while (eps <= abs(x0)) {
        x0 *= ((-1)*(x-1) * (i + 1)) / (i + 2); 	
        i++;
        cresult += x0;
    }
    double analyt = log(x);
    cout << "Analytic value: " << setprecision(10) << analyt << endl;
    cout << "Counted x on c++ = " << setprecision(10) << cresult << endl;
    cout << "n= " << i << endl;

    const double c1 = 1, c2 = 2;
    double res_asm= 0., x_ = x-1, x0_= x-1, n=0;
    
    __asm
    {
        finit; инициализация сопроцессора
        fld qword ptr[x_]; st(0) = x
        fstp x_; x_ = x

        fld qword ptr[x0_]; st(0) = x_1
        fld qword ptr[res_asm]; st(0) = res_asm, st(1) = x_1

        faddp st(1), st; st(0) = x_1
        fstp res_asm; res_asm = x_1

        begin_loop : ;сумма ряда

        fld qword ptr[x_]; st(0) = x_
        fchs; st(0) = -x_
        fld qword ptr[n]; st(0) = n, st(1) = -x_
        fld qword ptr[c1]; st(0) = 1, st(1) = n, st(2) = -x_
        faddp st(1), st; st(0) = n + 1, st(1) = -x_
        fmulp st(1), st; st(0) = (n + 1) * -x_
        fld qword ptr[n]; st(0) = n, st(1) = -(n + 1) * x_
        fld qword ptr[c2]; st(0) = 2, st(1) = n, st(2) = -x_ * (n + 1)
        faddp st(1), st; st(0) = n + 2, st(1) = -x_ * (n+ 1)
        fdivp st(1), st; st(0) = -x_ * (n + 1) / (n + 2) //=alfa 

        fld qword ptr[x0_]; st(0) = x_1, st(1) = -x1 * (n + 1) / (n + 2)
        fmulp st(1), st; st(0) = x_1 * -x_1 * (n + 1) / (n + 2)
        fst x0_
        fld qword ptr[res_asm]; st(0) = res_asm, st(1) = -x_ * (n + 1) / (n + 2)
        faddp st(1), st; st(0) = res_asm + -x_ * (n + 1) / (n + 2)
        fstp res_asm

        fld qword ptr[c1]; 
        fld qword ptr[n]; st(0) = n, st(1) = c1
            faddp st(1), st; st(0) = n + 1
            fstp n

            ;сравним член последовательности и епс
            fld qword ptr[x0_]; st(0) = x0
            fabs; st(0) = abs(x0)
            fld qword ptr[eps]; st(0) = eps, st(1) = x0
            fcomp st(1)
            fstsw ax
            sahf; Записать содержимое ah в flags
            fstp st(0)


            ja End; < конец
            jb begin_loop; < в цикл

            End:
    }
    

    cout << "ASM result: " << res_asm << endl;
    cout << "n= " << n << endl;
    cout << "Failability: " << abs(res_asm - analyt) << endl;
    return 0;
}
