// bitset standard header
#pragma once
#ifndef _BITSET_
#define _BITSET_
#ifndef RC_INVOKED
#include <string>
#include <iosfwd>

 #pragma pack(push,_CRT_PACKING)
 #pragma warning(push,3)
 #pragma push_macro("new")
 #undef new

 #pragma warning(disable: 4127)

 #pragma warning(disable: 6294)

_STD_BEGIN
		// TEMPLATE CLASS bitset
template<size_t _Bits>
	class bitset
	{	// store fixed-length sequence of Boolean elements
public:
	typedef typename _If<_Bits <= 32, _Uint32t, _ULonglong>::type _Ty;


		// CLASS reference
	class reference
		{	// proxy for an element
		friend class bitset<_Bits>;

	public:
		~reference() _NOEXCEPT
			{	// destroy the object
			}

		reference& operator=(bool _Val) _NOEXCEPT
			{	// assign Boolean to element
			_Pbitset->set(_Mypos, _Val);
			return (*this);
			}

		reference& operator=(const reference& _Bitref) _NOEXCEPT
			{	// assign reference to element
			_Pbitset->set(_Mypos, bool(_Bitref));
			return (*this);
			}

		reference& flip() _NOEXCEPT
			{	// complement stored element
			_Pbitset->flip(_Mypos);
			return (*this);
			}

		bool operator~() const _NOEXCEPT
			{	// return complemented element
			return (!_Pbitset->test(_Mypos));
			}

		operator bool() const _NOEXCEPT
			{	// return element
			return (_Pbitset->test(_Mypos));
			}

	private:
		reference() _NOEXCEPT
			: _Pbitset(0), _Mypos(0)
			{	// default construct
			}

		reference(bitset<_Bits>& _Bitset, size_t _Pos)
			: _Pbitset(&_Bitset), _Mypos(_Pos)
			{	// construct from bitset reference and position
			}

		bitset<_Bits> *_Pbitset;	// pointer to the bitset
		size_t _Mypos;	// position of element in bitset
		};

 #if _ITERATOR_DEBUG_LEVEL == 2
	static void _Validate(size_t _Pos)
		{	// verify that _Pos is within bounds
		if (_Bits <= _Pos)
			_DEBUG_ERROR("bitset index outside range");
		}

 #elif _ITERATOR_DEBUG_LEVEL == 1
	static void _Validate(size_t _Pos)
		{	// verify that _Pos is within bounds
		_SCL_SECURE_VALIDATE_RANGE(_Pos < _Bits);
		}

 #else /* _ITERATOR_DEBUG_LEVEL == 2 */
	static void _Validate(size_t)
		{	// (don't) verify that _Pos is within bounds
		}
 #endif /* _ITERATOR_DEBUG_LEVEL == 2 */

	constexpr bool _Subscript(size_t _Pos) const
		{	// subscript nonmutable sequence
                return ((_Array[_Pos / size_t(_Bitsperword)]
			& ((_Ty)1 << _Pos % _Bitsperword)) != 0);
		}

	constexpr bool operator[](size_t _Pos) const
		{	// subscript nonmutable sequence
 #if _ITERATOR_DEBUG_LEVEL == 0
		return (_Subscript(_Pos));

 #else /* _ITERATOR_DEBUG_LEVEL == 0 */
		return (_Bits <= _Pos
			? (_Validate(_Pos), false)
			: _Subscript(_Pos));
 #endif /* _ITERATOR_DEBUG_LEVEL == 0 */
		}

	reference operator[](size_t _Pos)
		{	// subscript mutable sequence
		_Validate(_Pos);
		return (reference(*this, _Pos));
		}

	constexpr bitset() _NOEXCEPT
		: _Array()
		{	// construct with all false values
		}

	static constexpr bool _Need_mask
		= _Bits < CHAR_BIT * sizeof (_ULonglong);

	static constexpr _ULonglong _Mask
		= (1ULL << (_Need_mask ? _Bits : 0)) - 1ULL;

	constexpr bitset(_ULonglong _Val) _NOEXCEPT
		: _Array{static_cast<_Ty>(_Need_mask ? _Val & _Mask : _Val)}
		{	// construct from bits in unsigned long long
		}

 #define _BITSET_SIZE_TYPE	\
	typename basic_string<_Elem, _Tr, _Alloc>::size_type

	template<class _Elem,
		class _Tr,
		class _Alloc>
		explicit bitset(const basic_string<_Elem, _Tr, _Alloc>& _Str,
			_BITSET_SIZE_TYPE _Pos = 0,
			_BITSET_SIZE_TYPE _Count = basic_string<_Elem, _Tr, _Alloc>::npos,
			_Elem _E0 = (_Elem)'0',
			_Elem _E1 = (_Elem)'1')
		{	// construct from [_Pos, _Pos + _Count) elements in string
		_Construct(_Str, _Pos, _Count, _E0, _E1);
		}

	template<class _Elem>
		explicit bitset(const _Elem *_Ptr,
			typename basic_string<_Elem>::size_type _Count =
				basic_string<_Elem>::npos,
			_Elem _E0 = (_Elem)'0',
			_Elem _E1 = (_Elem)'1')
		{	// initialize from NTBS
		_Construct(
			_Count == basic_string<_Elem>::npos
				? basic_string<_Elem>(_Ptr)
				: basic_string<_Elem>(_Ptr, _Count),
			0, _Count, _E0, _E1);
		}

	template<class _Elem,
		class _Tr,
		class _Alloc>
		void _Construct(
			const basic_string<_Elem, _Tr, _Alloc>& _Str,
			_BITSET_SIZE_TYPE _Pos,
			_BITSET_SIZE_TYPE _Count,
			_Elem _E0,
			_Elem _E1)
		{	// initialize from [_Pos, _Pos + _Count) elements in string
		if (_Str.size() < _Pos)
			_Xran();	// _Pos off end
		if (_Str.size() - _Pos < _Count)
			_Count = _Str.size() - _Pos;	// trim _Count to size

		typename basic_string<_Elem, _Tr, _Alloc>::size_type _Num;
		for (_Num = 0; _Num < _Count; ++_Num)
			if (!_Tr::eq(_Str[_Pos + _Num], _E0)
				&& !_Tr::eq(_Str[_Pos + _Num], _E1))
				_Xinv();

		if (_Bits < _Count)
			_Count = _Bits;	// trim _Count to length of bitset
		_Tidy();

		for (_Pos += _Count, _Num = 0; _Num < _Count; ++_Num)
			if (_Tr::eq(_Str[--_Pos], _E1))
				set(_Num);
		}

	bitset& operator&=(const bitset& _Right) _NOEXCEPT
		{	// AND in _Right
		for (ptrdiff_t _Wpos = _Words; 0 <= _Wpos; --_Wpos)
			_Array[_Wpos] &= _Right._Getword(_Wpos);
		return (*this);
		}

	bitset& operator|=(const bitset& _Right) _NOEXCEPT
		{	// OR in _Right
		for (ptrdiff_t _Wpos = _Words; 0 <= _Wpos; --_Wpos)
			_Array[_Wpos] |= _Right._Getword(_Wpos);
		return (*this);
		}

	bitset& operator^=(const bitset& _Right) _NOEXCEPT
		{	// XOR in _Right
		for (ptrdiff_t _Wpos = _Words; 0 <= _Wpos; --_Wpos)
			_Array[_Wpos] ^= _Right._Getword(_Wpos);
		return (*this);
		}

	bitset& operator<<=(size_t _Pos) _NOEXCEPT
		{	// shift left by _Pos
		const ptrdiff_t _Wordshift = (ptrdiff_t)(_Pos / _Bitsperword);
		if (_Wordshift != 0)
			for (ptrdiff_t _Wpos = _Words; 0 <= _Wpos; --_Wpos)
				_Array[_Wpos] = _Wordshift <= _Wpos	// shift by words
					? _Array[_Wpos - _Wordshift] : (_Ty)0;

		if ((_Pos %= _Bitsperword) != 0)
			{	// 0 < _Pos < _Bitsperword, shift by bits
			for (ptrdiff_t _Wpos = _Words; 0 < _Wpos; --_Wpos)
				_Array[_Wpos] = (_Ty)((_Array[_Wpos] << _Pos)
					| (_Array[_Wpos - 1] >> (_Bitsperword - _Pos)));
			_Array[0] <<= _Pos;
			}
		_Trim();
		return (*this);
		}

	bitset& operator>>=(size_t _Pos) _NOEXCEPT
		{	// shift right by _Pos, first by words then by bits
		const ptrdiff_t _Wordshift = (ptrdiff_t)(_Pos / _Bitsperword);
		if (_Wordshift != 0)
			for (ptrdiff_t _Wpos = 0; _Wpos <= _Words; ++_Wpos)
				_Array[_Wpos] = _Wordshift <= _Words - _Wpos
					? _Array[_Wpos + _Wordshift] : (_Ty)0;

		if ((_Pos %= _Bitsperword) != 0)
			{	// 0 < _Pos < _Bitsperword, shift by bits
			for (ptrdiff_t _Wpos = 0; _Wpos < _Words; ++_Wpos)
				_Array[_Wpos] = (_Ty)((_Array[_Wpos] >> _Pos)
					| (_Array[_Wpos + 1] << (_Bitsperword - _Pos)));
			_Array[_Words] >>= _Pos;
			}
		return (*this);
		}

	bitset& set() _NOEXCEPT
		{	// set all bits true
		_Tidy((_Ty)~0);
		return (*this);
		}

	bitset& set(size_t _Pos,
		bool _Val = true)
		{	// set bit at _Pos to _Val
		if (_Bits <= _Pos)
			_Xran();	// _Pos off end
		if (_Val)
                        _Array[_Pos / size_t(_Bitsperword)] |= (_Ty)1 << _Pos % _Bitsperword;
		else
                        _Array[_Pos / size_t(_Bitsperword)] &= ~((_Ty)1 << _Pos % _Bitsperword);
		return (*this);
		}

	bitset& reset() _NOEXCEPT
		{	// set all bits false
		_Tidy();
		return (*this);
		}

	bitset& reset(size_t _Pos)
		{	// set bit at _Pos to false
		return (set(_Pos, false));
		}

	bitset operator~() const _NOEXCEPT
		{	// flip all bits
		return (bitset(*this).flip());
		}

	bitset& flip() _NOEXCEPT
		{	// flip all bits
		for (ptrdiff_t _Wpos = _Words; 0 <= _Wpos; --_Wpos)
			_Array[_Wpos] = (_Ty)~_Array[_Wpos];

		_Trim();
		return (*this);
		}

	bitset& flip(size_t _Pos)
		{	// flip bit at _Pos
		if (_Bits <= _Pos)
			_Xran();	// _Pos off end
		_Array[_Pos / _Bitsperword] ^= (_Ty)1 << _Pos % _Bitsperword;
		return (*this);
		}

	unsigned long to_ulong() const
		{	// convert bitset to unsigned long
		_ULonglong _Val = to_ullong();
		unsigned long _Ans = (unsigned long)_Val;
		if (_Ans != _Val)
			_Xoflo();
		return (_Ans);
		}

	_ULonglong to_ullong() const
		{	// convert bitset to unsigned long long
		static_assert(sizeof (_ULonglong) % sizeof (_Ty) == 0,
			"unsigned long long not multiple of _Ty");

		ptrdiff_t _Wpos = _Words;
		for (; (ptrdiff_t)(sizeof (_ULonglong) / sizeof (_Ty)) <= _Wpos;
			--_Wpos)
			if (_Array[_Wpos] != 0)
				_Xoflo();	// fail if any high-order words are nonzero

		_ULonglong _Val = _Array[_Wpos];
		for (; 0 <= --_Wpos; )
			_Val = ((_Val << (_Bitsperword - 1)) << 1) | _Array[_Wpos];
		return (_Val);
		}

	template<class _Elem = char,
		class _Tr = char_traits<_Elem>,
		class _Alloc = allocator<_Elem> >
		basic_string<_Elem, _Tr, _Alloc>
			to_string(_Elem _E0 = (_Elem)'0',
				_Elem _E1 = (_Elem)'1') const
		{	// convert bitset to string
		basic_string<_Elem, _Tr, _Alloc> _Str;
		typename basic_string<_Elem, _Tr, _Alloc>::size_type _Pos;
		_Str.reserve(_Bits);

		for (_Pos = _Bits; 0 < _Pos; )
			if (test(--_Pos))
				_Str += _E1;
			else
				_Str += _E0;
		return (_Str);
		}

	size_t count() const _NOEXCEPT
		{	// count number of set bits
		const char *const _Bitsperbyte =
			"\0\1\1\2\1\2\2\3\1\2\2\3\2\3\3\4"
			"\1\2\2\3\2\3\3\4\2\3\3\4\3\4\4\5"
			"\1\2\2\3\2\3\3\4\2\3\3\4\3\4\4\5"
			"\2\3\3\4\3\4\4\5\3\4\4\5\4\5\5\6"
			"\1\2\2\3\2\3\3\4\2\3\3\4\3\4\4\5"
			"\2\3\3\4\3\4\4\5\3\4\4\5\4\5\5\6"
			"\2\3\3\4\3\4\4\5\3\4\4\5\4\5\5\6"
			"\3\4\4\5\4\5\5\6\4\5\5\6\5\6\6\7"
			"\1\2\2\3\2\3\3\4\2\3\3\4\3\4\4\5"
			"\2\3\3\4\3\4\4\5\3\4\4\5\4\5\5\6"
			"\2\3\3\4\3\4\4\5\3\4\4\5\4\5\5\6"
			"\3\4\4\5\4\5\5\6\4\5\5\6\5\6\6\7"
			"\2\3\3\4\3\4\4\5\3\4\4\5\4\5\5\6"
			"\3\4\4\5\4\5\5\6\4\5\5\6\5\6\6\7"
			"\3\4\4\5\4\5\5\6\4\5\5\6\5\6\6\7"
			"\4\5\5\6\5\6\6\7\5\6\6\7\6\7\7\x8";
		const unsigned char *_Ptr =
			(const unsigned char *)(const void *)_Array;
		const unsigned char *const _End = _Ptr + sizeof (_Array);
		size_t _Val = 0;
		for ( ; _Ptr != _End; ++_Ptr)
			_Val += _Bitsperbyte[*_Ptr];
		return (_Val);
		}

	constexpr size_t size() const _NOEXCEPT
		{	// return size of bitset
		return (_Bits);
		}

	size_t hash() const
		{	// hash bits to size_t value by pseudorandomizing transform
		return (_Hash_seq((const unsigned char *)_Array,
			sizeof (_Array)));
		}

	bool operator==(const bitset& _Right) const _NOEXCEPT
		{	// test for bitset equality
		for (ptrdiff_t _Wpos = _Words; 0 <= _Wpos; --_Wpos)
			if (_Array[_Wpos] != _Right._Getword(_Wpos))
				return (false);
		return (true);
		}

	bool operator!=(const bitset& _Right) const _NOEXCEPT
		{	// test for bitset inequality
		return (!(*this == _Right));
		}

	bool test(size_t _Pos) const
		{	// test if bit at _Pos is set
		if (_Bits <= _Pos)
			_Xran();	// _Pos off end
		return (_Subscript(_Pos));
		}

	bool any() const _NOEXCEPT
		{	// test if any bits are set
		for (ptrdiff_t _Wpos = _Words; 0 <= _Wpos; --_Wpos)
			if (_Array[_Wpos] != 0)
				return (true);
		return (false);
		}

	bool none() const _NOEXCEPT
		{	// test if no bits are set
		return (!any());
		}

	bool all() const _NOEXCEPT
		{	// test if all bits set
		return (count() == size());
		}

	bitset operator<<(size_t _Pos) const _NOEXCEPT
		{	// return bitset shifted left by _Pos
		return (bitset(*this) <<= _Pos);
		}

	bitset operator>>(size_t _Pos) const _NOEXCEPT
		{	// return bitset shifted right by _Pos
		return (bitset(*this) >>= _Pos);
		}

	_Ty _Getword(size_t _Wpos) const
		{	// get word at _Wpos
		return (_Array[_Wpos]);
		}

private:
	enum : ptrdiff_t

		{	// parameters for packing bits into words
		_Bitsperword = (ptrdiff_t)(CHAR_BIT * sizeof (_Ty)),
		_Words = (ptrdiff_t)(_Bits == 0
			? 0 : (_Bits - 1) / _Bitsperword)};	// NB: number of words - 1

	void _Tidy(_Ty _Wordval = 0)
		{	// set all words to _Wordval
		for (ptrdiff_t _Wpos = _Words; 0 <= _Wpos; --_Wpos)
			_Array[_Wpos] = _Wordval;
		if (_Wordval != 0)
			_Trim();
		}

	void _Trim()
		{	// clear any trailing bits in last word
		_Trim_if(bool_constant<_Bits == 0 || _Bits % _Bitsperword != 0>());
		}

	void _Trim_if(true_type)
		{	// bits to trim, remove them
		_Array[_Words] &= ((_Ty)1 << _Bits % _Bitsperword) - 1;
		}

	void _Trim_if(false_type)
		{	// no bits to trim, do nothing
		}

	[[noreturn]] void _Xinv() const
		{	// report invalid string element in bitset conversion
		_Xinvalid_argument("invalid bitset<N> char");
		}

	[[noreturn]] void _Xoflo() const
		{	// report converted value too big to represent
		_Xoverflow_error("bitset<N> overflow");
		}

	[[noreturn]] void _Xran() const
		{	// report bit index out of range
		_Xout_of_range("invalid bitset<N> position");
		}

	_Ty _Array[_Words + 1];	// the set of bits
	};

		// bitset TEMPLATE FUNCTIONS
template<size_t _Bits> inline
	bitset<_Bits> operator&(const bitset<_Bits>& _Left,
		const bitset<_Bits>& _Right) _NOEXCEPT
		{	// return bitset _Left AND _Right
		bitset<_Bits> _Ans = _Left;
		return (_Ans &= _Right);
		}

template<size_t _Bits> inline
	bitset<_Bits> operator|(const bitset<_Bits>& _Left,
		const bitset<_Bits>& _Right) _NOEXCEPT
		{	// return bitset _Left OR _Right
		bitset<_Bits> _Ans = _Left;
		return (_Ans |= _Right);
		}

template<size_t _Bits> inline
	bitset<_Bits> operator^(const bitset<_Bits>& _Left,
		const bitset<_Bits>& _Right) _NOEXCEPT
		{	// return bitset _Left XOR _Right
		bitset<_Bits> _Ans = _Left;
		return (_Ans ^= _Right);
		}

template<class _Elem,
	class _Tr,
	size_t _Bits> inline
	basic_ostream<_Elem, _Tr>& operator<<(
		basic_ostream<_Elem, _Tr>& _Ostr, const bitset<_Bits>& _Right)
	{	// insert bitset as a string
	const ctype<_Elem>& _Ctype_fac = _USE(_Ostr.getloc(), ctype<_Elem>);
	const _Elem _E0 = _Ctype_fac.widen('0');
	const _Elem _E1 = _Ctype_fac.widen('1');

	return (_Ostr
		<< _Right.template to_string<_Elem, _Tr, allocator<_Elem> >(
			_E0, _E1));
	}

		// TEMPLATE operator>>
template<class _Elem,
	class _Tr,
	size_t _Bits> inline
	basic_istream<_Elem, _Tr>& operator>>(
		basic_istream<_Elem, _Tr>& _Istr, bitset<_Bits>& _Right)
	{	// extract bitset as a string
	const ctype<_Elem>& _Ctype_fac = _USE(_Istr.getloc(), ctype<_Elem>);
	const _Elem _E0 = _Ctype_fac.widen('0');
	const _Elem _E1 = _Ctype_fac.widen('1');
	ios_base::iostate _State = ios_base::goodbit;
	bool _Changed = false;
	string _Str;
	const typename basic_istream<_Elem, _Tr>::sentry _Ok(_Istr);

	if (_Ok)
		{	// valid stream, extract elements
		_TRY_IO_BEGIN
		typename _Tr::int_type _Meta = _Istr.rdbuf()->sgetc();
		for (size_t _Count = _Right.size(); 0 < _Count;
			_Meta = _Istr.rdbuf()->snextc(), --_Count)
			{	// test _Meta
			_Elem _Char;
			if (_Tr::eq_int_type(_Tr::eof(), _Meta))
				{	// end of file, quit
				_State |= ios_base::eofbit;
				break;
				}
			else if ((_Char = _Tr::to_char_type(_Meta)) != _E0
				&& _Char != _E1)
				break;	// invalid element
			else if (_Str.max_size() <= _Str.size())
				{	// no room in string, give up (unlikely)
				_State |= ios_base::failbit;
				break;
				}
			else
				{	// valid, append '0' or '1'
				if (_Char == _E1)
					_Str.append(1, '1');
				else
					_Str.append(1, '0');
				_Changed = true;
				}
			}
		_CATCH_IO_(_Istr)
		}

	if (!_Changed)
		_State |= ios_base::failbit;
	_Istr.setstate(_State);
	_Right = bitset<_Bits>(_Str);	// convert string and store
	return (_Istr);
	}

	// TEMPLATE STRUCT SPECIALIZATION hash
template<size_t _Bits>
	struct hash<bitset<_Bits> >
	{	// hash functor for bitset<_Bits>
	typedef bitset<_Bits> argument_type;
	typedef size_t result_type;

	size_t operator()(const argument_type& _Keyval) const
		{	// hash _Keyval to size_t value by pseudorandomizing transform
		return (_Keyval.hash());
		}
	};
_STD_END

 #pragma pop_macro("new")
 #pragma warning(pop)
 #pragma pack(pop)
#endif /* RC_INVOKED */
#endif /* _BITSET_ */

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
