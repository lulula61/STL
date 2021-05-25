#ifndef CLASS_INTEGER_HPP_INCLUDED
#define CLASS_INTEGER_HPP_INCLUDED
class Integer {
private:
	int data;
public:
	Integer() {}
	Integer(const int &value) : data(value) {}
	Integer(const Integer &other) : data(other.data) {}
	bool operator==(const Integer &t)
	{
		return data == t.data;
	}
};


#endif // CLASS_INTEGER_HPP_INCLUDED
