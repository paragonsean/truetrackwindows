/*
the following error appears for the line
   b=0;

error C2679: binary '=' : no operator defined which takes a right-hand operand of type 'const int'
(or there is no acceptable conversion)
*/


class A
    {
    private:

    int w;

    public:

    void operator=( const int sorc ) { w = sorc; }
    };

class B : public A
    {
    public:
    void operator=( const int sorc ) { A::operator=(sorc); }
    };


/***********************************************************************
*                                 MAIN                                 *
***********************************************************************/
void main()
{
A a;
B b;

a = 0;
b = 0;
}

