#pragma once


struct Complex 
{
	double re, im;

	inline Complex () {}
	inline Complex (double re) : re(re), im(0.0) {}
	inline Complex (double re, double im) : re(re), im(im) {}

	inline double norm () const
	{
		return re*re + im*im;
	}
};

inline Complex operator ~ (const Complex& c)
{
	return Complex (c.re, -c.im);
}

inline Complex operator + (const Complex& l, const Complex& r)
{
	return Complex (l.re + r.re, l.im + r.im);
}

inline Complex operator - (const Complex& l, const Complex& r)
{
	return Complex (l.re - r.re, l.im - r.im);
}

inline Complex operator - (const Complex& c)
{
	return Complex (-c.re, -c.im);
}

inline Complex operator * (const Complex& l, const Complex& r)
{
	return Complex (l.re*r.re - l.im*r.im, l.re*r.im + l.im*r.re);
}

inline Complex operator / (const Complex& l, const Complex& r)
{
	Complex result (l * ~r);
	double den = 1.0/r.norm();
	return Complex(result.re*den, result.im*den);
}

inline Complex exp (const Complex& c) 
{
	double reexp = exp(c.re);
	return Complex(reexp*cos(c.im), reexp*sin(c.im));
}

const Complex C0 (0.0, 0.0);
const Complex CI (0.0, 1.0);
const Complex C1 (1.0, 0.0);