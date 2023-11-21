#include "mymatrix.h"

class PutNumF {
	static long long _capacity_max;
	char get_digit(int at)const {
		auto x = _val;
		--at;
		while (at)
		{
			--at;
			x /= 10;
		}
		return '0' + x % 10;
	}
	static int count_digits(long long _val) {
		auto x = _val;
		int cnt = 1;
		while (x/10)
		{
			x /= 10;
			++cnt;
		}
		return cnt;
	}
public:
	static int digit_max;
	static int cnt_digit_put;
	long long _val;
	PutNumF(long long Val) {
		_val = Val;
	}
	PutNumF(const PutNumF& b) {
		_val = b._val;
	}
	~PutNumF(){}
	void output_val(std::ostream& _stream)const
	{
		if (_val < _capacity_max&&_val>-_capacity_max/10) {
			_stream << _val;
			cnt_digit_put = count_digits(_val);
			if (_val < 0)cnt_digit_put++;
		}
		else {
			int digit_cnt = count_digits(_val);
			int _e_after =  digit_cnt - 1;
			int digit_c_remain = digit_max - 2 - count_digits(_e_after);
			cnt_digit_put = 2 + count_digits(_e_after) + 1;
			char cp;
			//put_first;
			if (_val < 0) {
				_stream.put('-'); 
				cnt_digit_put++;
					digit_c_remain--;
			}
			_stream.put(get_digit(digit_cnt));
			_stream.put('.');
			--digit_cnt;
			while (digit_c_remain>0)
			{
				cp = get_digit(digit_cnt);
				if (cp == '0')break;
				_stream.put(cp);
				--digit_cnt;
				--digit_c_remain;
				++cnt_digit_put;
			}
		}
	}
};

std::ostream& operator<<(std::ostream& _Stream, const PutNumF& _val) {
	_val.output_val(_Stream);
	return _Stream;
}

int PutNumF::digit_max = 7;
long long PutNumF::_capacity_max = 1000000;
int PutNumF::cnt_digit_put;

void Matrix::exchange_line(int l1, int l2)
{
	auto temp = mat[l1];
	mat[l1] = mat[l2];
	mat[l2] = temp;
}

void Matrix::row_echolon_form()
{
	int op_line = 0;
	for (int op_var_row = 0; op_var_row < seperate_index; ++op_var_row)
	{
		{
			auto temp = find_nozero_start_indix(op_line, op_var_row);
			if (temp == -1)continue;
			if (op_line != temp) {
				exchange_line(op_line, temp);
			}
		}
		if (mat[op_line][op_var_row] != 1) {
		this->operator[](op_line).scale(mat[op_line][op_var_row].reverse());
	}

		for (int i = op_line + 1; i < height; ++i)
		{
			if (mat[i][op_var_row] == 0)continue;
			auto temp = this->operator[](op_line).create_temp_line();
			temp.scale(mat[i][op_var_row]);
			this->operator[](i).substract(temp);
		}
		++op_line;
	}
}

void Matrix::reduced_row_echolon_form()
{
	int pivot_position;
	row_echolon_form();
	for (int i = height-1; i >0; --i)
	{
		pivot_position = nozero_entry(i);
		if (pivot_position == -1) {
			continue;
		}
		for (int _line = i -1; _line >= 0; --_line)
		{
			if (mat[_line][pivot_position] == 0)continue;
			auto temp = this->operator[](i).create_temp_line();
			temp.scale(mat[_line][pivot_position]);
			this->operator[](_line).substract(temp);
		}
	}
}

void Matrix::read_matrix_cin()
{
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)std::cin >> mat[i][j];
	}
}

void Matrix::print_matrix()const
{
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			format_output_val(std::cout, mat[i][j]);
			std::cout << ' ';
		}
		std::cout << std::endl;
	}
}

int Matrix::nozero_entry(int _at_l, int start_indix)
{
	int ret = -1;
	for (int i = start_indix; i < this->seperate_index; ++i)
	{
		if (mat[_at_l][i] != 0) {
			ret = i;
			break;
		}
	}
	return ret;
}

Matrix Matrix::inverse_mat()
{
	if (width!=height)
	{
		throw 100;
	}
	Matrix temp(width * 2, height);
	temp.copy_data(this->mat);
	//generate_identity_mat;
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j) {
			temp.mat[i][j + width] = i==j?MyNum(1):MyNum(0);
		}
	}
	temp.seperate_index = width;
	temp.row_echolon_form();
	temp.reduced_row_echolon_form();
	for (int i = 0; i < height; ++i)
	{
		if (temp.mat[i][i] != 1) {
			return Matrix(0, 0);
		}
	}
	Matrix ret(width, height);
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			ret.mat[i][j] = temp.mat[i][j + width];
		}
	}
	return ret;
}

Solution Matrix::analyze_result()
{
	Solution s;
	s.cnt_solution = width - 1;
	s.solution = Vector(s.cnt_solution);
	//check_incosisitance
	int cnt_zero_line = 0;
	for (int i = 0; i < height; i++)
	{
		if (nozero_entry(i) == -1) {
			if (mat[i][width - 1] != 0) {
				s.flag_inconsistance = true;
				return s;
			}
			cnt_zero_line++;
		}
	}
	s.set_free_var_number(s.cnt_solution - (height - cnt_zero_line));
	int* pivot_indix = new int[s.cnt_solution-s.cnt_free_var];
	int x = 0;

	for (int i = 0; i < height; i++)
	{
		auto _indix = nozero_entry(i);
		if (_indix == -1)continue;
		pivot_indix[x] = _indix;
		++x;
	}
	x = 0;
	//find position for free varable
	for (int i = 0; i < width-1; i++)
	{
		bool find = false;
		for (int j = 0; j < s.cnt_solution - s.cnt_free_var; j++)
		{
			if (pivot_indix[j] == i){
				find = true;
				break;
			}
		}
		if (!find) {
			s.free_var_id[x] = i;
			++x;
		}
	}
	//set_val zero;
	for (int i = 0; i < s.cnt_solution; i++)
	{
		s.solution[i] = 0;
		for (int j = 0; j < s.cnt_free_var; j++)
		{
			s.span[j][i] = 0;
		}
	}
	//find solution
	int solution_indix;
	for (int i = 0; i < height; i++)
	{
		solution_indix = nozero_entry(i);
		if (solution_indix == -1)continue;
		s.solution[solution_indix] = mat[i][width - 1];
		x = solution_indix;
		for (int j = 0; j < s.cnt_free_var; ++j) {
			++x;
			x = nozero_entry(i, x);
			if (x == -1)break;
			s.span[s.get_free_var_indix(x)][solution_indix] = -mat[i][x];
		}
		
	}

	for (int i = 0; i < s.cnt_free_var; ++i) {
		s.span[i][s.free_var_id[i]] += 1;
	}
	delete[] pivot_indix;
	return s;
}

Vector Matrix::extract_vector(int col)
{
	Vector ret(height);
	for (int i = 0; i < height; i++)
	{
		ret[i] = mat[i][col];
	}
	return ret;
}

Matrix Matrix::multipy_mat(const Matrix& b)const
{
	Matrix ret(b.width, this->height);
	int n = width;
	MyNum sum;
	for (int i = 0; i < ret.height; ++i)
	{
		for (int j = 0; j < ret.width; ++j) {
			sum = 0;
			for (int r = 0; r < n; ++r)
			{
				sum += mat[i][r] * b.mat[r][j];
			}
			ret.mat[i][j] = sum;
		}
	}
	return ret;
}

Vector Matrix::multipy_vec(const Vector& _vec) const
{
	Vector ret(height);
	MyNum sum;
	for (int i = 0; i < height; ++i)
	{
		sum = 0;
		for (int j = 0; j < width; ++j)
		{
			sum += mat[i][j] * _vec[j];
		}
		ret[i] = sum;
	}
	return ret;
}

Matrix Matrix::multipy_val(const MyNum _val)const
{
	Matrix ret(*this);
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			ret[i][j] *= _val;
		}
	}
	return ret;
}

void Matrix::_set_v_neg()
{
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			mat[i][j]._up = -mat[i][j]._up;
		}
	}
}

Matrix& Matrix::operator+=(const Matrix& b)
{
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
			mat[i][j] += b.mat[i][j];
	}
	return *this;
}

MyNum Matrix::det() const
{
	if (width != height)throw	"unable to calc det";
	MyNum sum(0);
	if (width == 2) {
		sum = (mat[0][0] * mat[1][1]) - (mat[1][0] * mat[0][1]);
	}
	else {
		Matrix mat_less(width - 1, height - 1);
		for (int i = 0; i < width; ++i) {
			if (mat[0][i] == 0)continue;
			for (int j = 1; j < height; ++j) {
				for (int x = 0; x < width; ++x) {
					if (x == i)continue;
					mat_less.mat[j - 1][(x > i ? x - 1 : x)]
						= mat[j][x];
				}
			}
			if(i%2)sum -= mat[0][i] * mat_less.det();
			else sum += mat[0][i] * mat_less.det();
		}
	}
	return sum;
}

Matrix Matrix::MinorsMatrix()
{
	Matrix ret(width, height);
	Matrix dimision_1(width - 1, height - 1);
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			dimision_1._copy_data_higher_dimision(mat, j, i);
			ret[i][j] = dimision_1.det();
		}
	}
	return ret;
}

Matrix Matrix::CofactorsMatrix()
{
	Matrix ret(width, height);
	Matrix dimision_1(width - 1, height - 1);
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			dimision_1._copy_data_higher_dimision(mat, j, i);
			if((i+j)%2==0)ret[i][j] = dimision_1.det();
			else ret[i][j] = -dimision_1.det();
		}
	}
	return ret;
}

void Matrix::SetDimision(int _Width, int _Height)
{
	_free_space();
	width = _Width;
	height = _Height;
	seperate_index = width - 1;
	_allocate_space();
}

Vector Matrix::ExtractRowVector(int i)
{
	if (i < 0 || i >= height)throw 8;
	return Vector(width, mat[i]);
}

void Matrix::_assign_c_vector(const Vector& _vec, int _i)
{
	for (int i = 0; i < height; ++i) {
		mat[i][_i] = _vec[i];
	}
}

MyNum* Matrix::lval_ref(int _i)
{
	return mat[_i];
}

int Matrix::find_nozero_start_indix(int start_at, int line_inidx)
{
	int ret = -1;
	for (int i  = start_at; i  < height; ++i )
	{
		if (mat[i][line_inidx] != 0) {
			ret = i; break;
		}
	}
	return ret;
}

void Matrix::_copy_data_higher_dimision(MyNum** _src, int ignore_row, int ignore_colum)
{
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			mat[i][j] = _src
				[i >= ignore_colum ? i + 1 : i][j >= ignore_row? j + 1: j];
		}
	}
}

void Matrix::_allocate_space()
{
	mat = (MyNum**)operator new(sizeof(MyNum*) * height);
	for (int i = 0; i < height; i++)
	{
		mat[i] = new MyNum[width];
	}
}

void Matrix::copy_data(MyNum** src)
{
	auto _size = sizeof(MyNum) * width;
	for (int i = 0; i < height; ++i)
	{
		memcpy_s(mat[i], _size, src[i], _size);
	}
}

void Matrix::_free_space()
{
	for (int i = 0; i < height; i++)
	{
		delete[] mat[i];
	}
	operator delete(mat);
}

Matrix::Matrix(int wid, int hei)
{
	width = wid;
	height = hei;
	seperate_index = width - 1;
	_allocate_space();
}

Matrix::Matrix(const Matrix& b)
{
	width = b.width;
	height = b.height;
	seperate_index = b.seperate_index;
	_allocate_space();
	copy_data(b.mat);
}

Matrix::Matrix(Matrix&& _right)noexcept
{
	width = _right.width;
	height = _right.height;
	mat = _right.mat;
	seperate_index = _right.seperate_index;
	_right.mat = nullptr;
	_right.width = _right.height = 0;
}

Matrix::~Matrix()
{
	if(mat)_free_space();
	mat = NULL;
}

const Matrix& Matrix::operator=(const Matrix& b)
{
	if (width != b.width || height != b.height) {
		_free_space();
		width = b.width;
		height = b.height;
		_allocate_space();
	}
	copy_data(b.mat);
	seperate_index = b.seperate_index;
	return *this;
}

const Matrix& Matrix::operator=(Matrix&& _right)noexcept
{
	_free_space();
	width = _right.width;
	_right.width = 0;
	height = _right.height;
	_right.height = 0;
	mat = _right.mat;
	_right.mat = nullptr;
	seperate_index = _right.seperate_index;
	return *this;
}

long long gcd(long long a, long long b)
{
	while (b^=a^=b^=a%=b);
	return a;
}

long long lcm(long long a, long long b)
{
	return a*b/gcd(a,b);
}

std::istream& operator>>(std::istream& _stream, MyNum& _val)
{
	char buffer[64];
	char _s1[32];
	int indix_of = 0;
	bool flag_nf = true;
	_stream >> buffer;
	while (buffer[indix_of])
	{
		if (buffer[indix_of] == '/') {
			flag_nf = false;
			break;
		}
		++indix_of;
	}
	if (flag_nf) {
		_val._down = 1;
		_val._up = _atoi64(buffer);
		return _stream;
	}
	strcpy_s(_s1, buffer + indix_of + 1);
	buffer[indix_of] = '\0';
	_val._down = _atoi64(_s1);
	_val._up = _atoi64(buffer);
	if (_val._down == 0)throw - 1;
	_val.simplefied();
	return _stream;
}

std::ostream& operator<<(std::ostream& _stream, const MyNum& _val)
{
	_stream << _val._up;
	if(_val._up!=0&&_val._down!=1)_stream <<'/' << _val._down;
	return _stream;
}

MyNum::MyNum(const MyNum& b)
{
	_up = b._up;
	_down = b._down;
}

const MyNum& MyNum::operator*=(const MyNum& b)
{
	_up *= b._up;
	_down *= b._down;
	simplefied();
	return *this;
}

MyNum MyNum::operator*(const MyNum& b)
{
	return MyNum(*this).operator*=(b);
}

const MyNum& MyNum::operator/=(const MyNum& b)
{
	if (b._up == 0)throw - 1;
	_up *= b._down;
	_down *= b._up;
	simplefied();
	return *this;
}

MyNum MyNum::operator/(const MyNum& b)
{
	return MyNum(*this).operator/=(b);
}

const MyNum& MyNum::operator+=(const MyNum& b)
{
	if (b._up == 0)return*this;
	_up *= b._down;
	_up += _down * b._up;
	_down *= b._down;
	simplefied();
	return *this;
}

MyNum MyNum::operator+(const MyNum& b)
{
	return MyNum(*this).operator+=(b);
}

const MyNum& MyNum::operator-=(const MyNum& b)
{
	if (b._up == 0)return*this;
	_up *= b._down;
	_up -= _down * b._up;
	_down *= b._down;
	simplefied();
	return *this;
}

MyNum MyNum::operator-(const MyNum& b)
{
	return MyNum(*this).operator-=(b);
}

void MyNum::simplefied()
{
	if (_up == 0) {
		_down = 1;
		return;
	}
	auto _gcd = gcd(abs(_up), _down);
	_up /= _gcd;
	_down /= _gcd;
}

bool MyNum::operator==(const MyNum& b) const
{
	auto _lcm = lcm(_down, b._down);
	auto a1 = this->_up * _lcm / this->_down;
	auto a2 = b._up * _lcm / b._down;
	return a1 == a2;
}

bool MyNum::operator!=(const MyNum& b) const
{
	auto _lcm = lcm(_down, b._down);
	auto a1 = this->_up * _lcm / this->_down;
	auto a2 = b._up * _lcm / b._down;
	return a1 != a2;
}

bool MyNum::operator<=(const MyNum& b) const
{
	auto _lcm = lcm(_down, b._down);
	auto a1 = this->_up * _lcm / this->_down;
	auto a2 = b._up * _lcm / b._down;
	return a1 <= a2;
}

bool MyNum::operator>=(const MyNum& b) const
{
	auto _lcm = lcm(_down, b._down);
	auto a1 = this->_up * _lcm / this->_down;
	auto a2 = b._up * _lcm / b._down;
	return a1 >= a2;
}

bool MyNum::operator<(const MyNum& b) const
{
	auto _lcm = lcm(_down, b._down);
	auto a1 = this->_up * _lcm / this->_down;
	auto a2 = b._up * _lcm / b._down;
	return a1 < a2;
}

bool MyNum::operator>(const MyNum& b) const
{
	auto _lcm = lcm(_down, b._down);
	auto a1 = this->_up * _lcm / this->_down;
	auto a2 = b._up * _lcm / b._down;
	return a1 > a2;
}

MyNum& MyNum::operator-()
{
	_up = -_up;
	return *this;
}

MyNum MyNum::reverse() const
{
	if (_up == 0)throw -1;

	return MyNum(_up<0?-_down:_down, abs(_up));
}

Matrix::Mat_line& Matrix::Mat_line::scale(const MyNum& _val)
{
	for (int i = 0; i < length; i++)
	{
		ptr[i] *= _val;
	}
	return *this;
}

Matrix::Mat_line::Mat_line(int wid)
{
	length = wid;
	ptr = new MyNum[wid];
	flag_temp_line = true;
}

Matrix::Mat_line& Matrix::Mat_line::add(const Mat_line& _val)
{
	for (int i = 0; i < length; i++)
	{
		ptr[i] += _val[i];
	}
	return *this;
}

Matrix::Mat_line& Matrix::Mat_line::substract(const Mat_line& _val)
{
	for (int i = 0; i < length; i++)
	{
		ptr[i] -= _val[i];
	}
	return *this;
}

Matrix::Mat_line Matrix::Mat_line::create_temp_line()
{
	Mat_line ret(length);
	for (int i = 0; i < length; i++)
	{
		ret.ptr[i] = ptr[i];
	}
	return ret;
}

void Matrix::format_output_val(std::ostream& _stream, const MyNum& _val)
{
	int cnt_space_put = 0;
	if (_val._down != 1) {
		_stream << PutNumF(_val._up) << '/';
		cnt_space_put += (PutNumF::digit_max - PutNumF::cnt_digit_put);
		_stream << PutNumF(_val._down);
		cnt_space_put += (PutNumF::digit_max - PutNumF::cnt_digit_put);
	}
	else {
		_stream << PutNumF(_val._up);
		cnt_space_put += (PutNumF::digit_max - PutNumF::cnt_digit_put);
		cnt_space_put += PutNumF::digit_max + 1;
	}
	for (int i = 0; i < cnt_space_put; ++i)_stream.put(' ');
}

void Vector::_allocate_space(MyNum* initialize_data)
{
	_vec = (MyNum*)operator new( sizeof(MyNum)*length);
	if (initialize_data)_copy_data(initialize_data);
}

void Vector::_copy_data(MyNum* _src)
{
	auto _size = sizeof(MyNum) * length;
	memcpy_s(_vec, _size, _src, _size);
}

void Vector::_discard_data()
{
	operator delete( _vec);
	_vec = nullptr;
}

Vector::Vector(int _size, MyNum* iptr)
{
	length = _size;
	_allocate_space(iptr);
}

Vector::Vector(Vector&& _Right) noexcept
{
	length = _Right.length;
	_Right.length = 0;
	_vec = _Right._vec;
	_Right._vec = NULL;
}

Vector::Vector(const Vector& _Left)
{
	length = _Left.length;
	_allocate_space();
	_copy_data(_Left._vec);
}

Vector::~Vector()
{
	_discard_data();
}

const Vector& Vector::operator=(const Vector& _Left)
{
	if (length != _Left.length) { 
		_discard_data(); 
		length = _Left.length;
		_allocate_space();
	}
	_copy_data(_Left._vec);
	return *this;
}

const Vector& Vector::operator=(Vector&& _Right)noexcept
{
	if (length)_discard_data();
	length = _Right.length;
	_Right.length = 0;
	_vec = _Right._vec;
	_Right._vec = NULL;
	return *this;
}

const Vector& Vector::operator+=(const Vector& b)
{
	if (length != b.length)throw 10;
	for (int i = 0; i < length; i++)
	{
		_vec[i] += b._vec[i];
	}
	return* this;
}

const Vector& Vector::operator-=(const Vector& b)
{
	if (length != b.length)throw 10;
	for (int i = 0; i < length; i++)
	{
		_vec[i] -= b._vec[i];
	}
	return*this;
}

const Vector& Vector::operator*=(const MyNum& _val)
{
	for (auto& a : *this) {
		a *= _val;
	}
	return *this;
}

Vector Vector::mutiply_num(MyNum _val)
{
	Vector ret(*this);
	for (auto& a : ret)a *= _val;
	return ret;
}

void Vector::_set_v_neg()
{
	for (int i = 0; i < length; ++i) {
		_vec[i]._up =-_vec[i]._up;
	}
}

void Solution::_discard_data()
{
	delete[] free_var_id;
	for (int i = 0; i < cnt_free_var; i++)
	{
		span[i].~Vector();
	}
	operator delete (span);
}

void Solution::_allocate_space()
{
	free_var_id = new int[cnt_free_var];
	span = (Vector*)operator new (sizeof(Vector) * cnt_free_var);
	for (int i = 0; i < cnt_free_var; i++)
	{
		new (&span[i])Vector(cnt_solution);
	}
}

Solution::Solution()
{
	span = NULL;
	free_var_id = NULL;
	cnt_free_var = cnt_solution = 0;
	flag_inconsistance = false;
}

Solution::Solution(const Solution& _left)
{
	cnt_free_var = _left.cnt_free_var;
	cnt_solution = _left.cnt_solution;
	solution = _left.solution;
	_allocate_space();
	for (int i = 0; i < cnt_free_var; i++)
	{
		span[i] = _left.span[i];
	}
	flag_inconsistance = _left.flag_inconsistance;
}

Solution::~Solution()
{
	_discard_data();
}

void Solution::_set_varable_cnt(int cnt)
{
	cnt_solution = cnt;
	solution = Vector(cnt);
}

int Solution::get_free_var_indix(int id)
{
	for (int i = 0; i < cnt_free_var; i++)
	{
		if (free_var_id[i] == id)return i;
	}
	return -1;
}

void Solution::set_free_var_number(int num)
{
	_discard_data();
	cnt_free_var = num;
	_allocate_space();
}

void Solution::print_result()const
{
	if (flag_inconsistance) {
		std::cout << "inconsistance, no solution!\n";
		return;
	}
	for (int i = 0; i < cnt_solution; i++)
	{
		std::cout << 'x' << i << " =";
		if (solution[i] != 0)std::cout <<' '<< solution[i];

		bool flag_free_var = false;
		for (int j = 0; j < cnt_free_var; j++) {
			if (i == free_var_id[j]) {
				std::cout << " free var\n";
				flag_free_var = true;
				break;
			}
		}
		if (flag_free_var) {
			continue;
		}
		for (int j = 0; j < cnt_free_var; j++)
		{
			if (span[j][i] == 0)continue;
			std::cout << ((span[j][i] >= 0) ? " +" : " ")
				<< span[j][i] << 'x' << free_var_id[j];
		}
		std::cout << std::endl;
	}
}
