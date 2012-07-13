/*
 * Copyright (c) 2011 - Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 */

#undef ctf_integer_nowrite
#define ctf_integer_nowrite(_type, _item, _src)			\
	_ctf_integer_ext(_type, _item, _src, BYTE_ORDER, 10, 0)

#undef ctf_float_nowrite
#define ctf_float_nowrite(_type, _item, _src)			\
	_ctf_float_nowrite(_type, _item, _src, 0)

#undef ctf_array_nowrite
#define ctf_array_nowrite(_type, _item, _src, _length)		\
	_ctf_array_encoded(_type, _item, _src, _length, none, 0)

#undef ctf_array_text_nowrite
#define ctf_array_text_nowrite(_type, _item, _src, _length)	\
	_ctf_array_encoded(_type, _item, _src, _length, UTF8, 0)

#undef ctf_sequence_nowrite
#define ctf_sequence_nowrite(_type, _item, _src, _length_type, _src_length) \
	_ctf_sequence_encoded(_type, _item, _src,		\
			_length_type, _src_length, none, 0)

#undef ctf_sequence_text_nowrite
#define ctf_sequence_text_nowrite(_type, _item, _src, _length_type, _src_length) \
	_ctf_sequence_encoded(_type, _item, _src,		\
			_length_type, _src_length, UTF8, 0)

#undef ctf_string_nowrite
#define ctf_string_nowrite(_item, _src)				\
	_ctf_string(_item, _src, 0)
