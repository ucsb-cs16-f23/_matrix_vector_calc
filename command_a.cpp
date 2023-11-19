#include "command_a.h"
#include<cstring>
#include<iostream>
#include"my_Vars.h"
#include<vector>

char cmNode::op_table[256];
using namespace std;
list<MyVar>mVar_list;
vector<FuncBase*> function_list;
char* _My_Stack_Poiner;
char* _m_stack_end;
char* _m_stack_begin;
void* FuncBase::ret_area;
char cmNode::ret_char_buffer[128];
char _m_shared_left[3] = "(";
char _m_shared_right[3] = ")";


int cmNode::mov_to_next(char*& _str)
{
	int cnt = 0;
	char* str = _str;
	bool flag_break = false;
	while (*str)
	{
		switch (op_table[*str])
		{
		case 5:
			++str;
			flag_break = true;
			break;
		case chBound:
			if (cnt == 0){
				++cnt;
				++str;
			}
			flag_break = true;
			break;
		case chOp_1:
		{
			if (cnt)flag_break=true;
			else {
				cnt = op_length(str);
				str += cnt;
				flag_break = true;
			}
		}
			break;
		default:
			break;
		}
		if (flag_break)break;
		++str;
		++cnt;
	}
	_str = str;
	return cnt;
}

void cmNode::clean_up()
{
	cmNode* ptr=this->pNext;
	cmNode* pFree;
	while (ptr)
	{
		pFree = ptr;
		ptr = ptr->pNext;
		delete pFree;
	}
}

cmNode::cmNode()
{
	pPrev = pNext = nullptr;
	type = idUNDEFINED;
	str = nullptr;
}

cmNode::cmNode(char* _str,int len)
{
	str = _str;
	pPrev = NULL;
	pNext = nullptr;
	length = len;
	switch (op_table[_str[0]])
	{
	case 0:
		type = mIDENTIFIER;
		break;
	case chOp_1:
		type = mOPERATOR;
		break;
	case chBound:
		type = mType::mExprEND;
		break;
	default:
		type = idUNDEFINED;
		break;
	}
	if (type == mIDENTIFIER) {
		if (str[0] >= '0' && str[0] <= '9'){
			type = mVALUE; 
			goto func_end;
		}
		get_cstr();
		for (auto& a : mVar_list) {
			if (strcmp(a.var_name, ret_char_buffer) == 0) {
				type = idVAR;
				goto func_end;
			}
		}
		for (auto a : function_list) {
			if (strcmp(a->name, ret_char_buffer) == 0) {
				type = idFUNC;
				goto func_end;
			}
		}
		type = idUNDEFINED;
	}
	if (type == mOPERATOR) {
		switch (str[0])
		{
		case '=':
			if (length == 2)type = mOPCMP;
			break;
		case '<':
		case '>':
			type = mOPCMP;
			break;
		case '!':
			type = mOPLOG;
			break;
		case '|':
		case '&':
			if (length == 2)type = mOPLOG;
			break;
		default:
			break;
		}
	}
	func_end:
	return;
}

cmNode::~cmNode()
{
	clean_up();
}

void cmNode::append(cmNode* nd)
{
	auto ptr = find_back();
	ptr->pNext = nd;
	nd->pPrev = ptr;
}

cmNode* cmNode::find_back()
{
	auto ptr = this;
	while ((ptr->pNext))
	{
		ptr = ptr->pNext;
	}
	return ptr;
}

cmNode* cmNode::find_head()
{
	auto ptr = this;
	while ((ptr->pPrev))
	{
		ptr = ptr->pPrev;
	}
	return ptr;
}

int cmNode::op_length(char* str)
{
	if (str[0] == '-') {
		if (str[1] && str[1] == '>')
			return 2;
	}
	if (str[0] == '=') {
		if (str[1] && str[1] == '=')
			return 2;
	}
	if (str[0] == '|') {
		if (str[1] && str[1] == '|')
			return 2;
	}
	if (str[0] == '<') {
		if (str[1] && str[1] == '=')
			return 2;
	}
	if (str[0] == '>') {
		if (str[1] && str[1] == '=')
			return 2;
	}
	if (str[0] == '&') {
		if (str[1] && str[1] == '&')
			return 2;
	}
	return 1;
}

cmNode* cmNode::create_list(char* _str)
{
	cmNode Head;
	cmNode* ptr = &Head;
	char* p_ass = _str;
	int cnt;
	while(*_str) {
		while (*_str==' ')++_str;//ignore space
		p_ass = _str;
		cnt = mov_to_next(_str);
		if (cnt == 0)continue;
		ptr->append(new cmNode(p_ass, cnt));
		ptr = ptr->pNext;
	}

	ptr = Head.pNext;
	ptr->_break_list();
	return ptr;
}

void cmNode::print_node()
{
	char buffer[32];
	cmNode* PTR = this;
	while (PTR)
	{
		memcpy_s(buffer, 32, PTR->str, PTR->length);
		buffer[PTR->length] = '\0';
		std::cout << buffer<<' ';
		PTR = PTR->pNext;
	}
	std::cout << std::endl;
}

void cmNode::_break_list()
{
	if (pPrev)pPrev->pNext = nullptr;
	pPrev = nullptr;
}

void cmNode::table_init()
{
	memset(op_table, 0, 256);
	op_set('=', chOp_1);
	op_set('+', chOp_1);
	op_set('-', chOp_1);
	op_set('*', chOp_1);
	op_set('/', chOp_1);
	op_set('(', chOp_1);
	op_set(')', chBound);
	op_set('>', chOp_1);
	op_set('<', chOp_1);
	op_set('|', chOp_1);
	op_set('&', chOp_1);
	op_set('?', chOp_1);
	op_set(']', chOp_1);
	op_set('[', chOp_1);
	op_set(':', chBound);

	op_set(',', chBound);
	op_set(' ', 5);
}

const char* cmNode::get_cstr() const
{
	memcpy_s(ret_char_buffer, 128, str, length);
	ret_char_buffer[length] = '\0';
	return ret_char_buffer;
}

void cmNode::insert(cmNode* _Node)
{
	if (pPrev) {
		pPrev->pNext = _Node;
	}
	_Node->pPrev = pPrev;
	pPrev = _Node;
	_Node->pNext = this;
}

FuncBase::FuncBase()
{
	name = new  char[30];
	strcpy_s(name, 30, "nana");
	cnt = 0;
	m_type_list = NULL;
}

FuncBase::FuncBase(const char* _name)
{
	auto len = strlen(_name) + 1;
	name = new  char[len];
	strcpy_s(name, len,_name);
	cnt = 0;
	m_type_list = NULL;
	is_ptr_list = NULL;
}

FuncBase::FuncBase(const FuncBase& b)
{
	auto len = strlen(b.name) + 1;
	name = new  char[len];
	strcpy_s(name, len, b.name);
	cnt = b.cnt;
	if (cnt) {
		m_type_list = new MyVar::_m_type[cnt];
		is_ptr_list = new bool[cnt];
		for (int i = 0; i < cnt; ++i) {
			m_type_list[i] = b.m_type_list[i];
			is_ptr_list[i] = b.is_ptr_list[i];
		}
		f_return_pointer = b.f_return_pointer;
	}
}

int FuncBase::get_arguments_cnts()
{
	return  cnt;
}

MyVar::_m_type FuncBase::get_arg_type(int a_at)
{
	return m_type_list[a_at];
}

void FunctionClassINIT(unsigned dwStackSize, unsigned dwRetAsize)
{
	_m_stack_end = (char*)operator new(dwStackSize);
	_m_stack_begin = _m_stack_end + dwStackSize;
	_My_Stack_Poiner = _m_stack_begin;
	FuncBase::ret_area = operator new(dwRetAsize);
}

void determine_sequence(cmNode* _expr) {
	cmNode* o_expr = _expr;
	while (_expr)
	{
		if (_expr->str[0] == '*' || _expr->str[0] == '/') {
			_expr->get_prev()->insert(new cmNode(_m_shared_left, 1));
			int cnt_left = 0;
			while (!(_expr->is_end()))
			{
				if (_expr->str[0] == '(') ++cnt_left;
				if (_expr->str[0] == ')') {
					if (cnt_left == 0)break;
					--cnt_left;
				}
				if (_expr->str[0] == '+'&&cnt_left == 0) break;
				if (_expr->str[0] == '-') {
					if (cnt_left == 0 &&
						_expr->get_prev()->str[0] != '*' &&
						_expr->get_prev()->str[0] != '/')break;
				}
				_expr = _expr->get_next();
			}
			if ((_expr->is_end()))_expr->append(new cmNode(_m_shared_right, 1));
			else _expr->insert(new cmNode(_m_shared_right, 1));

		}
		_expr = _expr->get_next();
	}
	_expr = o_expr;
	while (!(_expr->is_end()))
	{
		if (_expr->type == cmNode::mOPLOG && _expr->str[0] != '!') {
			cmNode* _x = _expr;
			_expr->insert(new cmNode(_m_shared_right, 1));
			_x = _x->get_prev()->get_prev();
			int rig_cnt = 0;
			while (!(_x->is_head()))
			{
				if (_x->str[0] == ')')++rig_cnt;
				if (_x->str[0] == '(') {
					if (rig_cnt)--rig_cnt;
					else break;
				}
				if (_x->type == cmNode::mOPLOG && _expr->str[0] != '!'
					&& rig_cnt == 0)break;
				_x = _x->get_prev();
			}
			if ((_x->is_head()))_x->insert(new cmNode(_m_shared_left, 1));
		}
		_expr = _expr->get_next();
	}
	while (!(_expr->is_head()))
	{
		if (_expr->type == cmNode::mOPLOG && _expr->str[0] != '!') {
			_expr = _expr->get_next();
			_expr->insert(new cmNode(_m_shared_left, 1));
			_expr->find_back()->append(new cmNode(_m_shared_right, 1));
		}
		_expr = _expr->get_prev();
	}
}

MyVar CreateVector(char* _A1)
{
	MyVar ret;
	int _cnt  = 0;
	char* ap;
	_VA_start(ap, _A1);
	while (is_VA_end(ap))
	{
		ap -= sizeof(MyVar);
		++_cnt;
	}
	_VA_start(ap,_A1);
	ret.assign_val(Vector(_cnt));
	for (auto& a : *ret.my_data.vec) {
		if (_VA_arg<MyVar>(ap).var_type != MyVar::number)
			throw "^CREATE_VECTOR^, unable to assign value";
		a = *_VA_arg<MyVar>(ap).my_data.num;
	}
	return ret;
}

void procCommand_1(char* _command)
{
	auto len = strlen(_command);
	cmNode* _expr = NULL;
	MyVar vr;
	if (len > 10 && strncmp(_command, "create_var", 10) == 0) {
		_command += 11;
		len = 0;
		while (_command[len] && _command[len] != ' ')
			++len;
		string str(_command, len);
		_command += len;
		mVar_list.push_back(MyVar(MyVar::null_var, str.c_str(), NULL));
		_expr = cmNode::create_list(_command);
		vr = mVar_list.back().assign_val(CalcExpr_L(_expr));
		goto func_end;
	}
	_expr = cmNode::create_list(_command);
	vr = CalcExpr_L(_expr);
func_end:
	switch (vr.var_type)
	{
	case MyVar::matrix:
		vr.my_data.mat->print_matrix();
		break;
	case MyVar::vector:
		for (auto& a : *vr.my_data.vec)cout << a << ' ';
		cout << endl;
		break;
	case MyVar::number:
		cout << vr.my_data.num[0] << endl;
		break;
	default:
		cout << endl;
	}
	tmp_var_list.clear();
}

MyVar SetSepIndix(Matrix* mat, MyNum indix) {
	mat->seperate_index = indix._up;
	MyVar ret;
	ret.assign_val<Matrix>(*mat);
	return ret;
}

MyVar CreateMatrix(char* _A1)
{
	MyVar ret;
	int _cnt = 0;
	char* ap;
	_VA_start(ap, _A1);
	int wid = _VA_arg<MyVar>(ap).my_data.num->_up;
	int hei = _VA_arg<MyVar>(ap).my_data.num->_up;
	ret.assign_val(Matrix(wid, hei));
	for (int i = 0; i < hei; ++i) {
		for (int j = 0; j < wid; ++j) {
			ret.my_data.mat->operator[](i)[j] = _VA_arg<MyVar>(ap).my_data.num[0];
		}
	}
	return ret;
}

void implFuncInit()
{
	function_list.push_back(new MYVA_FUNC(CreateVector, "CreateVector"));
	function_list.push_back(new MYFUNC(REF, "REF"));
	function_list.push_back(new MyFunc_VA<typename _Rt<decltype(CreateMatrix)>::type>(&CreateMatrix, "CreateMatrix"));
	function_list.push_back(new MYFUNC(SetSepIndix, "SetSepIndix"));
}

//IMPORTANT use a "varable stack" to support user define functions !
MyVar* FindVar(const char* name) {
	for (auto& a : mVar_list) {
		if (strcmp(a.var_name, name) == 0) {
			return&a;
		}
	}
	return 0;
}

//HELPER FUNCTION   (●'◡'●)
void _M_helper_calcexpr(MyVar* ret, bool& flag_neg, char& _m_op, MyVar& ptr_vr) {
	if (flag_neg)ptr_vr.set_negetive();
	switch (_m_op)
	{
	case '+':
		ret->op_add(ptr_vr);
		break;
	case '*':
		ret->op_mul(ptr_vr);
		break;
	case '/':
		ret->my_data.num->operator/=(*ptr_vr.my_data.num);
		break;
	default:
		break;
	}
	_m_op = '+';
	flag_neg = false;
};

//^^^ READY!^^^  <(￣︶￣)↗[GO!]
void _MS_pushVAL(MyVar& _val,bool is_ptr) {
	switch (_val.var_type)
	{
	case MyVar::number:
		if (is_ptr) PUSH<MyNum*>(_val.my_data.num);
		else PUSH<MyNum>(*_val.my_data.num);
		break;
	case MyVar::vector:
		if (is_ptr) PUSH<Vector*>(_val.my_data.vec);
		else PUSH<Vector>(*_val.my_data.vec);
		break;
	case MyVar::matrix:
		if (is_ptr) PUSH<Matrix*>(_val.my_data.mat);
		else PUSH<Matrix>(*_val.my_data.mat);
		break;
	default:
		break;
	}
}

//Push rvalue
void _MS_pushVAL_R(MyVar&& _val) {
	switch (_val.var_type)
	{
	case MyVar::number:
		PUSH<MyNum>(std::move(*_val.my_data.num));
		break;
	case MyVar::vector:
		PUSH<Vector>(std::move(*_val.my_data.vec));
		break;
	case MyVar::matrix:
		 PUSH<Matrix>(std::move(*_val.my_data.mat));
		break;
	default:
		break;
	}
}

void ExxCallFunc(cmNode*& ptr_expr, FuncBase* fptr) {
	//passing arguments
	cmNode* _expr = ptr_expr;
	if ((fptr->_is_va_func()))goto _call_va_func;
	for (int i = 0; i < fptr->get_arguments_cnts(); ++i) {
		if ((fptr->arg_is_pointer_at(i))) {
			//arg is "reference", lvalue required
			MyVar* x = FindVar(_expr->get_cstr());
			if (x == nullptr || _expr->get_next()->type != cmNode::mExprEND) {
				throw "func require lvalue";
			}
			if (fptr->get_arg_type(i) != x->var_type)throw "Function BAD call with error arg type";
			_MS_pushVAL(*x, true);
			_expr = _expr->get_next();
			if (!(_expr->is_end()))_expr = _expr->get_next();
		}
		else {
			MyVar&& x = CalcExpr_L(_expr);
			if(i!=fptr->get_arguments_cnts()-1)_expr = _expr->get_next();
			if (fptr->get_arg_type(i) != x.var_type)throw "Function BAD call with error arg type";
			_MS_pushVAL_R(std::move(x));
		}
	}
	goto _func_ret;
_call_va_func:
	fptr->record_stack_pointer();
	while (true)
	{
		PUSH(CalcExpr_L(_expr));
		if (_expr->str[0] == ')')break;
		_expr = _expr->get_next();
	}
	_func_ret:
	ptr_expr = _expr;
	//call Function;
	fptr->_call_func_ret_on_retarea();
	return;
}

//Function Caller for rvalue return function (*/ω＼*)
//* if the function returns lvalue, it treated as rvalue function
MyVar RvalRetFunctionCaller(cmNode*& ptr_expr) {
	cmNode* _expr = ptr_expr;
	FuncBase* fptr = NULL;
	//find function//
	{
		auto cname = _expr->get_cstr();
		for (auto a : function_list) {
			if (strcmp(cname, a->name)==0) {
				fptr = a;
				break;
			}
		}
	}
	_expr = _expr->get_next()->get_next();
	ExxCallFunc(_expr, fptr);
	ptr_expr = _expr;
	MyVar ret;
	if (fptr->f_return_pointer) ret = **reinterpret_cast<MyVar**>(FuncBase::ret_area);
	else ret = ::std::move(*reinterpret_cast<MyVar*>(FuncBase::ret_area));
	return ret;
}
//wwwwwwwww~

MyVar* LvalFuncCaller(cmNode*& ptr_expr) {
	cmNode* _expr = ptr_expr;
	FuncBase* fptr = NULL;
	{
		auto cname = _expr->get_cstr();
		for (auto a : function_list) {
			if (strcmp(cname, a->name)==0) {
				fptr = a;
				break;
			}
		}
	}
	_expr = _expr->get_next()->get_next();
	ExxCallFunc(_expr, fptr);
	ptr_expr = _expr;
	return *reinterpret_cast<MyVar**>(FuncBase::ret_area);
}

MyVar CalcExpr_L(cmNode*& ptr_expr) {
	MyVar *m_lvalue = NULL;
	cmNode* _expr = ptr_expr;

	//^^^ find '=' ^^^
	//GET lvalue, if no lvalue (no '=' ) lvlaue == null
	while ((_expr) && _expr->type != cmNode::mExprEND) {

		if (_expr->str[0] == '(')break;
		//treated as rval ,for case such as "a = (b = c) + 1;"

		if (_expr->str[0] == '='&&_expr->length==1) {
			auto cnme = ptr_expr->get_cstr();
			if (ptr_expr->type == cmNode::idFUNC) {
				m_lvalue = LvalFuncCaller(ptr_expr);
			}
			else 
				m_lvalue = FindVar(cnme);
			_expr = _expr->get_next();
			break;
		}
		_expr = _expr->get_next();
	}

	MyVar ret;

	//calculate rval (●'◡'●)
	if (m_lvalue) {
		ret = m_lvalue->assign_val(std::move(CalcExpr_L(_expr)));
	}
	else {
		_expr = ptr_expr;
		ret.assign_val(CalcExpr_R(_expr));
	}
	ptr_expr = _expr;
	return ret;
}

void _expr_end(cmNode*& ptr_expr) {
	cmNode* _expr = ptr_expr;
	int cnt_left = 0;
	while (_expr&&(cnt_left > 0 || _expr->type != cmNode::mExprEND))
	{
		if (_expr->str[0] == '(')++cnt_left;
		if (_expr->str[0] == ')')--cnt_left;
		_expr = _expr->get_next();
	}
	ptr_expr = _expr;
}

//assume function is rvalue 
MyVar CalcExpr_R(cmNode*& ptr_expr)
{
	MyVar ret;
	cmNode* _expr = ptr_expr;
	MyVar tmp_val_r;
	bool flag_neg = false;
	char _m_op = '+';
	bool flag_not = false;

	//go through expression from left to right  (o゜▽゜)o☆
	while ((_expr) && _expr->type!=cmNode::mExprEND)
	{
		switch (_expr->type)
		{
		case cmNode::mType::idVAR:
			tmp_val_r.assign_val<MyVar>(*FindVar(_expr->get_cstr()));
			if (flag_not)flag_not = false, tmp_val_r._Not();
			_M_helper_calcexpr(&ret, flag_neg, _m_op, tmp_val_r);
			break;
		case cmNode::mType::idFUNC:
			tmp_val_r.assign_val(RvalRetFunctionCaller(_expr));
			if (flag_not)flag_not = false, tmp_val_r._Not();
			_M_helper_calcexpr(&ret, flag_neg, _m_op, tmp_val_r);
			break;
		case cmNode::mType::mVALUE:
			tmp_val_r.assign_val(MyNum(atoll(_expr->get_cstr())));
			if (flag_not)flag_not = false, tmp_val_r._Not();//!1 ? is that  meaningful??
			_M_helper_calcexpr(&ret, flag_neg, _m_op, tmp_val_r);
			break;
		case cmNode::mType::mOPERATOR:
		{
			switch (_expr->str[0])
			{
			case '-':
				flag_neg = !flag_neg;
				break;
			case '(':
				_expr = _expr->get_next();
				tmp_val_r = CalcExpr_L(_expr);
				if (flag_not)flag_not = false, tmp_val_r._Not();
				_M_helper_calcexpr(&ret, flag_neg, _m_op, tmp_val_r);
				//recursion to calculate value q(≧▽≦q)
				break;
			case '?':
				_expr = _expr->get_next();
				if ((ret.my_data.num->_up)) {
					//(//true? (...) : (...) )
					ret.assign_val(CalcExpr_R(_expr));
					_expr = _expr->get_next();
					_expr_end(_expr);
				}
				else {
					while (_expr->str[0]!=':')
					{
						_expr = _expr->get_next();
					}
					_expr = _expr->get_next();
					ret.assign_val(CalcExpr_R(_expr));
				}
				goto func_return;
				break;
			case '[':
				//TODO: 
				break;
			case ']':
				goto func_return;
			case '{':
				//TODO
				break;
			case '}':
				//TODO
				break;
			default:
				_m_op = _expr->str[0];
				break;
			}
		}
			break;
			case cmNode::mOPCMP:
				switch (_expr->str[0])
				{
				case '<':
					_expr = _expr->get_next();
					tmp_val_r.assign_val(CalcExpr_R(_expr));
					if (ret.var_type != MyVar::number &&
						tmp_val_r.var_type != MyVar::number)throw "cannot compare...";
					if (_expr->length == 2)
						ret.assign_val(MyNum(ret.my_data.num->operator<=(
							*tmp_val_r.my_data.num)));
					else
						ret.assign_val(MyNum(ret.my_data.num->operator<(
							*tmp_val_r.my_data.num)));
					goto func_return;
					break;
				case '>':
					_expr = _expr->get_next();
					tmp_val_r.assign_val(CalcExpr_R(_expr));
					if (ret.var_type != MyVar::number &&
						tmp_val_r.var_type != MyVar::number)throw "cannot compare...";
					if (_expr->length == 2)
						ret.assign_val(MyNum(ret.my_data.num->operator>=(
							*tmp_val_r.my_data.num)));
					else 
						ret.assign_val(MyNum(ret.my_data.num->operator>(
							*tmp_val_r.my_data.num)));
					goto func_return;
					break;
				case '=':
					tmp_val_r.assign_val(CalcExpr_R(_expr));
					ret.assign_val(MyNum(
						(int)(ret == tmp_val_r)
					));
					if (flag_not)flag_not = false, ret._Not();
					goto func_return;
					break;
				}
				break;
			case cmNode::mOPLOG:
				switch (_expr->str[0])
				{
				case '|':
					_expr = _expr->get_next();
					if (_expr->length == 2) {
						if (ret.my_data.num->_up != 0) {
							_expr = _expr->get_next();
							_expr_end(_expr);
							break;
						}
						tmp_val_r.assign_val(CalcExpr_R(_expr));
						ret.assign_val(MyNum(
							ret.my_data.num->_up || tmp_val_r.my_data.num->_up
						));
					}
					break;
				case '&':
					_expr = _expr->get_next();
					if (_expr->length == 2) {
						if (ret.my_data.num->_up == 0) {
							_expr = _expr->get_next();
							_expr_end(_expr);
							break;
						}
						tmp_val_r.assign_val(CalcExpr_R(_expr));
						ret.assign_val(MyNum(
							ret.my_data.num->_up || tmp_val_r.my_data.num->_up
						));
					}
					break;
				case '!':
					flag_not = !flag_not;
					break;
				default:
					break;
				}
				break;
		default:
			break;
		}
		if(_expr)_expr = _expr->get_next();
	}
	//prepare to return, set pointer ヾ(≧▽≦*)o
	func_return:
	ptr_expr = _expr;
	return ret;
}