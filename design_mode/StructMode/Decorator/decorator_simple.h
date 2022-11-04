#include "iostream"
#include "string"
using namespace std;



//����1 ����һ���ӿ�
class Shape
{
public:
    virtual void draw() = 0;
    virtual ~Shape(){}
};



//����2 ����ʵ�ֽӿڵ�ʵ����
class Rectangle : public Shape
{
public:
    void draw()
    {
        cout << "Shape: Rectangle" << endl;
    };
};

class Circle : public Shape
{
public:
    void draw()
    {
        cout << "Shape: Circle" << endl;
    };
};



//����3 ����ʵ���� Shape �ӿڵĳ���װ����
class ShapeDecorator : public Shape
{
protected:
    Shape* decoratedShape;
};



//����4 ������չ�� ShapeDecorator ���ʵ��װ����
class RedShapeDecorator : public ShapeDecorator
{
public:
    RedShapeDecorator(Shape* decoratedShape)    //���캯��
    {
        this->decoratedShape = decoratedShape;
    }

    void draw()
    {
        this->decoratedShape->draw();
        this->setRedBorder();
    }
private:
    void setRedBorder()
    {
        cout << "Border Color: Red" << endl;
    }
};



//����5 ʹ�� RedShapeDecorator ��װ�� Shape ����
void TestDecoratorSimple()
{
    Shape* circle = new Circle;
    ShapeDecorator* redCircle = new RedShapeDecorator( new Circle );
    ShapeDecorator* redRectangle = new RedShapeDecorator( new Rectangle );

    cout << "Circle with normal border" <<endl;
    circle->draw();

    cout << "\nCircle of red border" <<endl;
    redCircle->draw();

    cout << "\nRectangle of red border" <<endl;
    redRectangle->draw();


    delete circle;
    delete redCircle;
    delete redRectangle;

    system("pause");
}