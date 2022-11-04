#include "iostream"
#include "string"
using namespace std;



//步骤1 创建一个接口
class Shape
{
public:
    virtual void draw() = 0;
    virtual ~Shape(){}
};



//步骤2 创建实现接口的实体类
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



//步骤3 创建实现了 Shape 接口的抽象装饰类
class ShapeDecorator : public Shape
{
protected:
    Shape* decoratedShape;
};



//步骤4 创建扩展了 ShapeDecorator 类的实体装饰类
class RedShapeDecorator : public ShapeDecorator
{
public:
    RedShapeDecorator(Shape* decoratedShape)    //构造函数
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



//步骤5 使用 RedShapeDecorator 来装饰 Shape 对象
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