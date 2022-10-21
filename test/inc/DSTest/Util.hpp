#ifndef DSTEST_UTIL
#define DSTEST_UTIL

#include <Stream/InOut.hpp>
#include <DP/Factory.hpp>
#include <iostream>
#include <string>

namespace DSTest {

#define toS() [[nodiscard]] std::string toString() const;
#define vtoS() [[nodiscard]] virtual std::string toString() const;
#define atoS() [[nodiscard]] virtual std::string toString() const = 0;
#define otoS() [[nodiscard]] std::string toString() const override;
#define itoS(B) }; \
std::string B::toString() const { return #B; }

#define DC(B) B(B const&) = default;
#define NC(B) B(B const&) = delete;
#define DM(B) B(B&&) = default;
#define NM(B) B(B&&) = delete;

#define Header(B) struct B {
#define HeaderD(D, B) Header(D##B : B)
#define HeaderV(B) Header(V##B)
#define HeaderDV(D, B) Header(D##V##B : V##B)
#define HeaderA(B) Header(A##B)
#define HeaderDA(D, B) Header(D##A##B : A##B)

#define Footer(B) toS() \
itoS(B)
#define FooterD(D, B) Footer(D##B)
#define FooterV(B) vtoS() \
itoS(V##B)
#define FooterDV(D, B) otoS() \
itoS(D##V##B)
#define FooterA(B) atoS() \
itoS(A##B)
#define FooterDA(D, B) otoS() \
itoS(D##A##B)

#define HF(B) Header(B) \
Footer(B)
#define HFD(D, B) HeaderD(D, B) \
FooterD(D, B)
#define HFV(B) HeaderV(B) \
FooterV(B)
#define HFDV(D, B) HeaderDV(D, B) \
FooterDV(D, B)
#define HFA(B) HeaderA(B) \
FooterA(B)
#define HFDA(D, B) HeaderDA(D, B) \
FooterDA(D, B)

#define HXF(B, X) Header(B##X) \
	X(B##X) \
Footer(B##X)
#define HXFD(D, B, X) HeaderD(D, B##X) \
	X(D##B##X) \
FooterD(D, B##X)
#define HXFV(B, X) HeaderV(B##X) \
	X(V##B##X) \
FooterV(B##X)
#define HXFDV(D, B, X) HeaderDV(D, B##X) \
	X(D##V##B##X) \
FooterDV(D, B##X)
#define HXFA(B, X) HeaderA(B##X) \
	X(A##B##X) \
FooterA(B##X)
#define HXFDA(D, B, X) HeaderDA(D, B##X) \
	X(D##A##B##X) \
FooterDA(D, B##X)

/*
(.*)\((.*)\) \\
(.*)\:\:(.*)\(pv p\, qv q\, rv r\) \: (.*)\(p\), (.*)\(q\), (.*)\(r\) \{\}

	bool operator==($4 const&) const noexcept; \\
$1\($2\) \\
$3::$4(pv p, qv q, rv r) : $5(p), $6(q), $7(r) {} \\
bool $4::operator==($4 const& other) const noexcept \\
{ return $5 == other.$5 && $6 == other.$6 && $7 == other.$7; }
*/
bool operator==(Stream::Input const&, Stream::Input const&) noexcept
{ return true; }


#define HPF(B, PN, pn, pv, p) Header(B##PN) \
	pn m##PN; \
	B##PN(pv p); \
	bool operator==(B##PN const&) const noexcept; \
	bool operator==(pv) const noexcept; \
Footer(B##PN) \
B##PN::B##PN(pv p) : m##PN(p) {} \
bool B##PN::operator==(B##PN const& other) const noexcept \
{ return m##PN == other.m##PN; } \
bool B##PN::operator==(pv other) const noexcept \
{ return m##PN == other; }
#define HPFD(D, B, PN, pn, pv, p) HeaderD(D, B##PN) \
	D##B##PN(pv p); \
FooterD(D, B##PN) \
D##B##PN::D##B##PN(pv p) : B##PN(p) {}

#define HPFV(B, PN, pn, pv, p) HeaderV(B##PN) \
	pn m##PN; \
	V##B##PN(pv p); \
	bool operator==(V##B##PN const&) const noexcept; \
	bool operator==(pv) const noexcept; \
FooterV(B##PN) \
V##B##PN::V##B##PN(pv p) : m##PN(p) {} \
bool V##B##PN::operator==(V##B##PN const& other) const noexcept \
{ return m##PN == other.m##PN; } \
bool V##B##PN::operator==(pv other) const noexcept \
{ return m##PN == other; }
#define HPFDV(D, B, PN, pn, pv, p) HeaderDV(D, B##PN) \
	D##V##B##PN(pv p); \
FooterDV(D, B##PN) \
D##V##B##PN::D##V##B##PN(pv p) : V##B##PN(p) {}

#define HPFA(B, PN, pn, pv, p) HeaderA(B##PN) \
	pn m##PN; \
	A##B##PN(pv p); \
	bool operator==(A##B##PN const&) const noexcept; \
	bool operator==(pv) const noexcept; \
FooterA(B##PN) \
A##B##PN::A##B##PN(pv p) : m##PN(p) {} \
bool A##B##PN::operator==(A##B##PN const& other) const noexcept \
{ return m##PN == other.m##PN; } \
bool A##B##PN::operator==(pv other) const noexcept \
{ return m##PN == other; }
#define HPFDA(D, B, PN, pn, pv, p) HeaderDA(D, B##PN) \
	D##A##B##PN(pv p); \
FooterDA(D, B##PN) \
D##A##B##PN::D##A##B##PN(pv p) : A##B##PN(p) {}

#define HPXF(B, PN, pn, pv, p, X) Header(B##PN##X) \
	pn m##PN; \
	B##PN##X(pv p); \
	X(B##PN##X) \
	bool operator==(B##PN##X const&) const noexcept; \
	bool operator==(pv) const noexcept; \
Footer(B##PN##X) \
B##PN##X::B##PN##X(pv p) : m##PN(p) {} \
bool B##PN##X::operator==(B##PN##X const& other) const noexcept \
{ return m##PN == other.m##PN; } \
bool B##PN##X::operator==(pv other) const noexcept \
{ return m##PN == other; }
#define HPXFD(D, B, PN, pn, pv, p, X) HeaderD(D, B##PN##X) \
	D##B##PN##X(pv p); \
	X(D##B##PN##X) \
FooterD(D, B##PN##X) \
D##B##PN##X::D##B##PN##X(pv p) : B##PN##X(p) {}

#define HPXFV(B, PN, pn, pv, p, X) HeaderV(B##PN##X) \
	pn m##PN; \
	V##B##PN##X(pv p); \
	X(V##B##PN##X) \
	bool operator==(V##B##PN##X const&) const noexcept; \
	bool operator==(pv) const noexcept; \
FooterV(B##PN##X) \
V##B##PN##X::V##B##PN##X(pv p) : m##PN(p) {} \
bool V##B##PN##X::operator==(V##B##PN##X const& other) const noexcept \
{ return m##PN == other.m##PN; } \
bool V##B##PN##X::operator==(pv other) const noexcept \
{ return m##PN == other; }
#define HPXFDV(D, B, PN, pn, pv, p, X) HeaderDV(D, B##PN##X) \
	D##V##B##PN##X(pv p); \
	X(D##V##B##PN##X) \
FooterDV(D, B##PN##X) \
D##V##B##PN##X::D##V##B##PN##X(pv p) : V##B##PN##X(p) {}

#define HPXFA(B, PN, pn, pv, p, X) HeaderA(B##PN##X) \
	pn m##PN; \
	A##B##PN##X(pv p); \
	X(A##B##PN##X) \
	bool operator==(A##B##PN##X const&) const noexcept; \
	bool operator==(pv) const noexcept; \
FooterA(B##PN##X) \
A##B##PN##X::A##B##PN##X(pv p) : m##PN(p) {} \
bool A##B##PN##X::operator==(A##B##PN##X const& other) const noexcept \
{ return m##PN == other.m##PN; } \
bool A##B##PN##X::operator==(pv other) const noexcept \
{ return m##PN == other; }
#define HPXFDA(D, B, PN, pn, pv, p, X) HeaderDA(D, B##PN##X) \
	D##A##B##PN##X(pv p); \
	X(D##A##B##PN##X) \
FooterDA(D, B##PN##X) \
D##A##B##PN##X::D##A##B##PN##X(pv p) : A##B##PN##X(p) {}

#define HPQF(B, PN, pn, pv, p, QN, qn, qv, q) Header(B##PN##QN) \
	pn m##PN; \
	qn m##QN; \
	B##PN##QN(pv p, qv q); \
	bool operator==(B##PN##QN const&) const noexcept; \
	bool operator==(pv) const noexcept; \
	bool operator==(qv) const noexcept; \
Footer(B##PN##QN) \
B##PN##QN::B##PN##QN(pv p, qv q) : m##PN(p), m##QN(q) {} \
bool B##PN##QN::operator==(B##PN##QN const& other) const noexcept \
{ return m##PN == other.m##PN && m##QN == other.m##QN; } \
bool B##PN##QN::operator==(pv other) const noexcept \
{ return m##PN == other; } \
bool B##PN##QN::operator==(qv other) const noexcept \
{ return m##QN == other; }
#define HPQFD(D, B, PN, pn, pv, p, QN, qn, qv, q) HeaderD(D, B##PN##QN) \
	D##B##PN##QN(pv p, qv q); \
FooterD(D, B##PN##QN) \
D##B##PN##QN::D##B##PN##QN(pv p, qv q) : B##PN##QN(p, q) {}

#define HPQFV(B, PN, pn, pv, p, QN, qn, qv, q) HeaderV(B##PN##QN) \
	pn m##PN; \
	qn m##QN; \
	V##B##PN##QN(pv p, qv q); \
	bool operator==(V##B##PN##QN const&) const noexcept; \
	bool operator==(pv) const noexcept; \
	bool operator==(qv) const noexcept; \
FooterV(B##PN##QN) \
V##B##PN##QN::V##B##PN##QN(pv p, qv q) : m##PN(p), m##QN(q) {} \
bool V##B##PN##QN::operator==(V##B##PN##QN const& other) const noexcept \
{ return m##PN == other.m##PN && m##QN == other.m##QN; } \
bool V##B##PN##QN::operator==(pv other) const noexcept \
{ return m##PN == other; } \
bool V##B##PN##QN::operator==(qv other) const noexcept \
{ return m##QN == other; }
#define HPQFDV(D, B, PN, pn, pv, p, QN, qn, qv, q) HeaderDV(D, B##PN##QN) \
	D##V##B##PN##QN(pv p, qv q); \
FooterDV(D, B##PN##QN) \
D##V##B##PN##QN::D##V##B##PN##QN(pv p, qv q) : V##B##PN##QN(p, q) {}

#define HPQFA(B, PN, pn, pv, p, QN, qn, qv, q) HeaderA(B##PN##QN) \
	pn m##PN; \
	qn m##QN; \
	A##B##PN##QN(pv p, qv q); \
	bool operator==(A##B##PN##QN const&) const noexcept; \
	bool operator==(pv) const noexcept; \
	bool operator==(qv) const noexcept; \
FooterA(B##PN##QN) \
A##B##PN##QN::A##B##PN##QN(pv p, qv q) : m##PN(p), m##QN(q) {} \
bool A##B##PN##QN::operator==(A##B##PN##QN const& other) const noexcept \
{ return m##PN == other.m##PN && m##QN == other.m##QN; } \
bool A##B##PN##QN::operator==(pv other) const noexcept \
{ return m##PN == other; } \
bool A##B##PN##QN::operator==(qv other) const noexcept \
{ return m##QN == other; }
#define HPQFDA(D, B, PN, pn, pv, p, QN, qn, qv, q) HeaderDA(D, B##PN##QN) \
	D##A##B##PN##QN(pv p, qv q); \
FooterDA(D, B##PN##QN) \
D##A##B##PN##QN::D##A##B##PN##QN(pv p, qv q) : A##B##PN##QN(p, q) {}

#define HPQXF(B, PN, pn, pv, p, QN, qn, qv, q, X) Header(B##PN##QN##X) \
	pn m##PN; \
	qn m##QN; \
	B##PN##QN##X(pv p, qv q); \
	X(B##PN##QN##X) \
	bool operator==(B##PN##QN##X const&) const noexcept; \
	bool operator==(pv) const noexcept; \
	bool operator==(qv) const noexcept; \
Footer(B##PN##QN##X) \
B##PN##QN##X::B##PN##QN##X(pv p, qv q) : m##PN(p), m##QN(q) {} \
bool B##PN##QN##X::operator==(B##PN##QN##X const& other) const noexcept \
{ return m##PN == other.m##PN && m##QN == other.m##QN; } \
bool B##PN##QN##X::operator==(pv other) const noexcept \
{ return m##PN == other; } \
bool B##PN##QN##X::operator==(qv other) const noexcept \
{ return m##QN == other; }
#define HPQXFD(D, B, PN, pn, pv, p, QN, qn, qv, q, X) HeaderD(D, B##PN##QN##X) \
	D##B##PN##QN##X(pv p, qv q); \
	X(D##B##PN##QN##X) \
FooterD(D, B##PN##QN##X) \
D##B##PN##QN##X::D##B##PN##QN##X(pv p, qv q) : B##PN##QN##X(p, q) {}

#define HPQXFV(B, PN, pn, pv, p, QN, qn, qv, q, X) HeaderV(B##PN##QN##X) \
	pn m##PN; \
	qn m##QN; \
	V##B##PN##QN##X(pv p, qv q); \
	X(V##B##PN##QN##X) \
	bool operator==(V##B##PN##QN##X const&) const noexcept; \
	bool operator==(pv) const noexcept; \
	bool operator==(qv) const noexcept; \
FooterV(B##PN##QN##X) \
V##B##PN##QN##X::V##B##PN##QN##X(pv p, qv q) : m##PN(p), m##QN(q) {} \
bool V##B##PN##QN##X::operator==(V##B##PN##QN##X const& other) const noexcept \
{ return m##PN == other.m##PN && m##QN == other.m##QN; } \
bool V##B##PN##QN##X::operator==(pv other) const noexcept \
{ return m##PN == other; } \
bool V##B##PN##QN##X::operator==(qv other) const noexcept \
{ return m##QN == other; }
#define HPQXFDV(D, B, PN, pn, pv, p, QN, qn, qv, q, X) HeaderDV(D, B##PN##QN##X) \
	D##V##B##PN##QN##X(pv p, qv q); \
	X(D##V##B##PN##QN##X) \
FooterDV(D, B##PN##QN##X) \
D##V##B##PN##QN##X::D##V##B##PN##QN##X(pv p, qv q) : V##B##PN##QN##X(p, q) {}

#define HPQXFA(B, PN, pn, pv, p, QN, qn, qv, q, X) HeaderA(B##PN##QN##X) \
	pn m##PN; \
	qn m##QN; \
	A##B##PN##QN##X(pv p, qv q); \
	X(A##B##PN##QN##X) \
	bool operator==(A##B##PN##QN##X const&) const noexcept; \
	bool operator==(pv) const noexcept; \
	bool operator==(qv) const noexcept; \
FooterA(B##PN##QN##X) \
A##B##PN##QN##X::A##B##PN##QN##X(pv p, qv q) : m##PN(p), m##QN(q) {} \
bool A##B##PN##QN##X::operator==(A##B##PN##QN##X const& other) const noexcept \
{ return m##PN == other.m##PN && m##QN == other.m##QN; } \
bool A##B##PN##QN##X::operator==(pv other) const noexcept \
{ return m##PN == other; } \
bool A##B##PN##QN##X::operator==(qv other) const noexcept \
{ return m##QN == other; }
#define HPQXFDA(D, B, PN, pn, pv, p, QN, qn, qv, q, X) HeaderDA(D, B##PN##QN##X) \
	D##A##B##PN##QN##X(pv p, qv q); \
	X(D##A##B##PN##QN##X) \
FooterDA(D, B##PN##QN##X) \
D##A##B##PN##QN##X::D##A##B##PN##QN##X(pv p, qv q) : A##B##PN##QN##X(p, q) {}

#define HPQRF(B, PN, pn, pv, p, QN, qn, qv, q, RN, rn, rv, r) Header(B##PN##QN##RN) \
	pn m##PN; \
	qn m##QN; \
	rn m##RN; \
	B##PN##QN##RN(pv p, qv q, rv r); \
	bool operator==(B##PN##QN##RN const&) const noexcept; \
	bool operator==(pv) const noexcept; \
	bool operator==(qv) const noexcept; \
	bool operator==(rv) const noexcept; \
Footer(B##PN##QN##RN) \
B##PN##QN##RN::B##PN##QN##RN(pv p, qv q, rv r) : m##PN(p), m##QN(q), m##RN(r) {} \
bool B##PN##QN##RN::operator==(B##PN##QN##RN const& other) const noexcept \
{ return m##PN == other.m##PN && m##QN == other.m##QN && m##RN == other.m##RN; } \
bool B##PN##QN##RN::operator==(pv other) const noexcept \
{ return m##PN == other; } \
bool B##PN##QN##RN::operator==(qv other) const noexcept \
{ return m##QN == other; } \
bool B##PN##QN##RN::operator==(rv other) const noexcept \
{ return m##RN == other; }
#define HPQRFD(D, B, PN, pn, pv, p, QN, qn, qv, q, RN, rn, rv, r) HeaderD(D, B##PN##QN##RN) \
	D##B##PN##QN##RN(pv p, qv q, rv r); \
FooterD(D, B##PN##QN##RN) \
D##B##PN##QN##RN::D##B##PN##QN##RN(pv p, qv q, rv r) : B##PN##QN##RN(p, q, r) {}

#define HPQRFV(B, PN, pn, pv, p, QN, qn, qv, q, RN, rn, rv, r) HeaderV(B##PN##QN##RN) \
	pn m##PN; \
	qn m##QN; \
	rn m##RN; \
	V##B##PN##QN##RN(pv p, qv q, rv r); \
	bool operator==(V##B##PN##QN##RN const&) const noexcept; \
	bool operator==(pv) const noexcept; \
	bool operator==(qv) const noexcept; \
	bool operator==(rv) const noexcept; \
FooterV(B##PN##QN##RN) \
V##B##PN##QN##RN::V##B##PN##QN##RN(pv p, qv q, rv r) : m##PN(p), m##QN(q), m##RN(r) {} \
bool V##B##PN##QN##RN::operator==(V##B##PN##QN##RN const& other) const noexcept \
{ return m##PN == other.m##PN && m##QN == other.m##QN && m##RN == other.m##RN; } \
bool V##B##PN##QN##RN::operator==(pv other) const noexcept \
{ return m##PN == other; } \
bool V##B##PN##QN##RN::operator==(qv other) const noexcept \
{ return m##QN == other; } \
bool V##B##PN##QN##RN::operator==(rv other) const noexcept \
{ return m##RN == other; }
#define HPQRFDV(D, B, PN, pn, pv, p, QN, qn, qv, q, RN, rn, rv, r) HeaderDV(D, B##PN##QN##RN) \
	D##V##B##PN##QN##RN(pv p, qv q, rv r); \
FooterDV(D, B##PN##QN##RN) \
D##V##B##PN##QN##RN::D##V##B##PN##QN##RN(pv p, qv q, rv r) : V##B##PN##QN##RN(p, q, r) {}

#define HPQRFA(B, PN, pn, pv, p, QN, qn, qv, q, RN, rn, rv, r) HeaderA(B##PN##QN##RN) \
	pn m##PN; \
	qn m##QN; \
	rn m##RN; \
	A##B##PN##QN##RN(pv p, qv q, rv r); \
	bool operator==(A##B##PN##QN##RN const&) const noexcept; \
	bool operator==(pv) const noexcept; \
	bool operator==(qv) const noexcept; \
	bool operator==(rv) const noexcept; \
FooterA(B##PN##QN##RN) \
A##B##PN##QN##RN::A##B##PN##QN##RN(pv p, qv q, rv r) : m##PN(p), m##QN(q), m##RN(r) {} \
bool A##B##PN##QN##RN::operator==(A##B##PN##QN##RN const& other) const noexcept \
{ return m##PN == other.m##PN && m##QN == other.m##QN && m##RN == other.m##RN; } \
bool A##B##PN##QN##RN::operator==(pv other) const noexcept \
{ return m##PN == other; } \
bool A##B##PN##QN##RN::operator==(qv other) const noexcept \
{ return m##QN == other; } \
bool A##B##PN##QN##RN::operator==(rv other) const noexcept \
{ return m##RN == other; }
#define HPQRFDA(D, B, PN, pn, pv, p, QN, qn, qv, q, RN, rn, rv, r) HeaderDA(D, B##PN##QN##RN) \
	D##A##B##PN##QN##RN(pv p, qv q, rv r); \
FooterDA(D, B##PN##QN##RN) \
D##A##B##PN##QN##RN::D##A##B##PN##QN##RN(pv p, qv q, rv r) : A##B##PN##QN##RN(p, q, r) {}

#define HPQRXF(B, PN, pn, pv, p, QN, qn, qv, q, RN, rn, rv, r, X) Header(B##PN##QN##RN##X) \
	pn m##PN; \
	qn m##QN; \
	rn m##RN; \
	B##PN##QN##RN##X(pv p, qv q, rv r); \
	X(B##PN##QN##RN##X) \
	bool operator==(B##PN##QN##RN##X const&) const noexcept; \
	bool operator==(pv) const noexcept; \
	bool operator==(qv) const noexcept; \
	bool operator==(rv) const noexcept; \
Footer(B##PN##QN##RN##X) \
B##PN##QN##RN##X::B##PN##QN##RN##X(pv p, qv q, rv r) : m##PN(p), m##QN(q), m##RN(r) {} \
bool B##PN##QN##RN##X::operator==(B##PN##QN##RN##X const& other) const noexcept \
{ return m##PN == other.m##PN && m##QN == other.m##QN && m##RN == other.m##RN; } \
bool B##PN##QN##RN##X::operator==(pv other) const noexcept \
{ return m##PN == other; } \
bool B##PN##QN##RN##X::operator==(qv other) const noexcept \
{ return m##QN == other; } \
bool B##PN##QN##RN##X::operator==(rv other) const noexcept \
{ return m##RN == other; }
#define HPQRXFD(D, B, PN, pn, pv, p, QN, qn, qv, q, RN, rn, rv, r, X) HeaderD(D, B##PN##QN##RN##X) \
	D##B##PN##QN##RN##X(pv p, qv q, rv r); \
	X(D##B##PN##QN##RN##X) \
FooterD(D, B##PN##QN##RN##X) \
D##B##PN##QN##RN##X::D##B##PN##QN##RN##X(pv p, qv q, rv r) : B##PN##QN##RN##X(p, q, r) {}

#define HPQRXFV(B, PN, pn, pv, p, QN, qn, qv, q, RN, rn, rv, r, X) HeaderV(B##PN##QN##RN##X) \
	pn m##PN; \
	qn m##QN; \
	rn m##RN; \
	V##B##PN##QN##RN##X(pv p, qv q, rv r); \
X(V##B##PN##QN##RN##X) \
	bool operator==(V##B##PN##QN##RN##X const&) const noexcept; \
	bool operator==(pv) const noexcept; \
	bool operator==(qv) const noexcept; \
	bool operator==(rv) const noexcept; \
FooterV(B##PN##QN##RN##X) \
V##B##PN##QN##RN##X::V##B##PN##QN##RN##X(pv p, qv q, rv r) : m##PN(p), m##QN(q), m##RN(r) {} \
bool V##B##PN##QN##RN##X::operator==(V##B##PN##QN##RN##X const& other) const noexcept \
{ return m##PN == other.m##PN && m##QN == other.m##QN && m##RN == other.m##RN; } \
bool V##B##PN##QN##RN##X::operator==(pv other) const noexcept \
{ return m##PN == other; } \
bool V##B##PN##QN##RN##X::operator==(qv other) const noexcept \
{ return m##QN == other; } \
bool V##B##PN##QN##RN##X::operator==(rv other) const noexcept \
{ return m##RN == other; }
#define HPQRXFDV(D, B, PN, pn, pv, p, QN, qn, qv, q, RN, rn, rv, r, X) HeaderDV(D, B##PN##QN##RN##X) \
	D##V##B##PN##QN##RN##X(pv p, qv q, rv r); \
	X(D##V##B##PN##QN##RN##X) \
FooterDV(D, B##PN##QN##RN##X) \
D##V##B##PN##QN##RN##X::D##V##B##PN##QN##RN##X(pv p, qv q, rv r) : V##B##PN##QN##RN##X(p, q, r) {}

#define HPQRXFA(B, PN, pn, pv, p, QN, qn, qv, q, RN, rn, rv, r, X) HeaderA(B##PN##QN##RN##X) \
	pn m##PN; \
	qn m##QN; \
	rn m##RN; \
	A##B##PN##QN##RN##X(pv p, qv q, rv r); \
	X(A##B##PN##QN##RN##X) \
	bool operator==(A##B##PN##QN##RN##X const&) const noexcept; \
	bool operator==(pv) const noexcept; \
	bool operator==(qv) const noexcept; \
	bool operator==(rv) const noexcept; \
FooterA(B##PN##QN##RN##X) \
A##B##PN##QN##RN##X::A##B##PN##QN##RN##X(pv p, qv q, rv r) : m##PN(p), m##QN(q), m##RN(r) {} \
bool A##B##PN##QN##RN##X::operator==(A##B##PN##QN##RN##X const& other) const noexcept \
{ return m##PN == other.m##PN && m##QN == other.m##QN && m##RN == other.m##RN; } \
bool A##B##PN##QN##RN##X::operator==(pv other) const noexcept \
{ return m##PN == other; } \
bool A##B##PN##QN##RN##X::operator==(qv other) const noexcept \
{ return m##QN == other; } \
bool A##B##PN##QN##RN##X::operator==(rv other) const noexcept \
{ return m##RN == other; }
#define HPQRXFDA(D, B, PN, pn, pv, p, QN, qn, qv, q, RN, rn, rv, r, X) HeaderDA(D, B##PN##QN##RN##X) \
	D##A##B##PN##QN##RN##X(pv p, qv q, rv r); \
	X(D##A##B##PN##QN##RN##X) \
FooterDA(D, B##PN##QN##RN##X) \
D##A##B##PN##QN##RN##X::D##A##B##PN##QN##RN##X(pv p, qv q, rv r) : A##B##PN##QN##RN##X(p, q, r) {}

#define StructNF(B) \
HF(B) \
HFD(D1, B) \
HFD(D2, B) \
HFV(B) \
HFDV(D1, B) \
HFDV(D2, B) \
HFA(B) \
HFDA(D1, B) \
HFDA(D2, B)
#define Struct(B) \
StructNF(B) \
struct FV##B : DP::Factory<V##B, int> {}; \
struct FA##B : DP::Factory<A##B, int> {};

#define StructXNF(B, X) \
HXF(B, X) \
HXFD(D1, B, X) \
HXFD(D2, B, X) \
HXFV(B, X) \
HXFDV(D1, B, X) \
HXFDV(D2, B, X) \
HXFA(B, X) \
HXFDA(D1, B, X) \
HXFDA(D2, B, X)
#define StructX(B, X) \
StructXNF(B, X) \
struct FV##B##X : DP::Factory<V##B##X, int> {}; \
struct FA##B##X : DP::Factory<A##B##X, int> {};

#define StructPNF(B, PN, pn, pv, p) \
HPF(B, PN, pn, pv, p) \
HPFD(D1, B, PN, pn, pv, p) \
HPFD(D2, B, PN, pn, pv, p) \
HPFV(B, PN, pn, pv, p) \
HPFDV(D1, B, PN, pn, pv, p) \
HPFDV(D2, B, PN, pn, pv, p) \
HPFA(B, PN, pn, pv, p) \
HPFDA(D1, B, PN, pn, pv, p) \
HPFDA(D2, B, PN, pn, pv, p)
#define StructP(B, PN, pn, pv, p) \
StructPNF(B, PN, pn, pv, p) \
struct FV##B##PN : DP::Factory<V##B##PN, int, pn> {}; \
struct FA##B##PN : DP::Factory<A##B##PN, int, pn> {};

#define StructPXNF(B, PN, pn, pv, p, X) \
HPXF(B, PN, pn, pv, p, X) \
HPXFD(D1, B, PN, pn, pv, p, X) \
HPXFD(D2, B, PN, pn, pv, p, X) \
HPXFV(B, PN, pn, pv, p, X) \
HPXFDV(D1, B, PN, pn, pv, p, X) \
HPXFDV(D2, B, PN, pn, pv, p, X) \
HPXFA(B, PN, pn, pv, p, X) \
HPXFDA(D1, B, PN, pn, pv, p, X) \
HPXFDA(D2, B, PN, pn, pv, p, X)
#define StructPX(B, PN, pn, pv, p, X) \
StructPXNF(B, PN, pn, pv, p, X) \
struct FV##B##PN##X : DP::Factory<V##B##PN##X, int, pn> {}; \
struct FA##B##PN##X : DP::Factory<A##B##PN##X, int, pn> {};

#define StructPQNF(B, PN, pn, pv, p, QN, qn, qv, q) \
HPQF(B, PN, pn, pv, p, QN, qn, qv, q) \
HPQFD(D1, B, PN, pn, pv, p, QN, qn, qv, q) \
HPQFD(D2, B, PN, pn, pv, p, QN, qn, qv, q) \
HPQFV(B, PN, pn, pv, p, QN, qn, qv, q) \
HPQFDV(D1, B, PN, pn, pv, p, QN, qn, qv, q) \
HPQFDV(D2, B, PN, pn, pv, p, QN, qn, qv, q) \
HPQFA(B, PN, pn, pv, p, QN, qn, qv, q) \
HPQFDA(D1, B, PN, pn, pv, p, QN, qn, qv, q) \
HPQFDA(D2, B, PN, pn, pv, p, QN, qn, qv, q)
#define StructPQ(B, PN, pn, pv, p, QN, qn, qv, q) \
StructPQNF(B, PN, pn, pv, p, QN, qn, qv, q) \
struct FV##B##PN##QN : DP::Factory<V##B##PN##QN, int, pn, qn> {}; \
struct FA##B##PN##QN : DP::Factory<A##B##PN##QN, int, pn, qn> {};

#define StructPQXNF(B, PN, pn, pv, p, QN, qn, qv, q, X) \
HPQXF(B, PN, pn, pv, p, QN, qn, qv, q, X) \
HPQXFD(D1, B, PN, pn, pv, p, QN, qn, qv, q, X) \
HPQXFD(D2, B, PN, pn, pv, p, QN, qn, qv, q, X) \
HPQXFV(B, PN, pn, pv, p, QN, qn, qv, q, X) \
HPQXFDV(D1, B, PN, pn, pv, p, QN, qn, qv, q, X) \
HPQXFDV(D2, B, PN, pn, pv, p, QN, qn, qv, q, X) \
HPQXFA(B, PN, pn, pv, p, QN, qn, qv, q, X) \
HPQXFDA(D1, B, PN, pn, pv, p, QN, qn, qv, q, X) \
HPQXFDA(D2, B, PN, pn, pv, p, QN, qn, qv, q, X)
#define StructPQX(B, PN, pn, pv, p, QN, qn, qv, q, X) \
StructPQXNF(B, PN, pn, pv, p, QN, qn, qv, q, X) \
struct FV##B##PN##QN##X : DP::Factory<V##B##PN##QN##X, int, pn, qn> {}; \
struct FA##B##PN##QN##X : DP::Factory<A##B##PN##QN##X, int, pn, qn> {};

#define StructPQRNF(B, PN, pn, pv, p, QN, qn, qv, q, RN, rn, rv, r) \
HPQRF(B, PN, pn, pv, p, QN, qn, qv, q, RN, rn, rv, r) \
HPQRFD(D1, B, PN, pn, pv, p, QN, qn, qv, q, RN, rn, rv, r) \
HPQRFD(D2, B, PN, pn, pv, p, QN, qn, qv, q, RN, rn, rv, r) \
HPQRFV(B, PN, pn, pv, p, QN, qn, qv, q, RN, rn, rv, r) \
HPQRFDV(D1, B, PN, pn, pv, p, QN, qn, qv, q, RN, rn, rv, r) \
HPQRFDV(D2, B, PN, pn, pv, p, QN, qn, qv, q, RN, rn, rv, r) \
HPQRFA(B, PN, pn, pv, p, QN, qn, qv, q, RN, rn, rv, r) \
HPQRFDA(D1, B, PN, pn, pv, p, QN, qn, qv, q, RN, rn, rv, r) \
HPQRFDA(D2, B, PN, pn, pv, p, QN, qn, qv, q, RN, rn, rv, r)
#define StructPQR(B, PN, pn, pv, p, QN, qn, qv, q, RN, rn, rv, r) \
StructPQRNF(B, PN, pn, pv, p, QN, qn, qv, q, RN, rn, rv, r) \
struct FV##B##PN##QN##RN : DP::Factory<V##B##PN##QN##RN, int, pn, qn, rn> {}; \
struct FA##B##PN##QN##RN : DP::Factory<A##B##PN##QN##RN, int, pn, qn, rn> {};

#define StructPQRXNF(B, PN, pn, pv, p, QN, qn, qv, q, RN, rn, rv, r, X) \
HPQRXF(B, PN, pn, pv, p, QN, qn, qv, q, RN, rn, rv, r, X) \
HPQRXFD(D1, B, PN, pn, pv, p, QN, qn, qv, q, RN, rn, rv, r, X) \
HPQRXFD(D2, B, PN, pn, pv, p, QN, qn, qv, q, RN, rn, rv, r, X) \
HPQRXFV(B, PN, pn, pv, p, QN, qn, qv, q, RN, rn, rv, r, X) \
HPQRXFDV(D1, B, PN, pn, pv, p, QN, qn, qv, q, RN, rn, rv, r, X) \
HPQRXFDV(D2, B, PN, pn, pv, p, QN, qn, qv, q, RN, rn, rv, r, X) \
HPQRXFA(B, PN, pn, pv, p, QN, qn, qv, q, RN, rn, rv, r, X) \
HPQRXFDA(D1, B, PN, pn, pv, p, QN, qn, qv, q, RN, rn, rv, r, X) \
HPQRXFDA(D2, B, PN, pn, pv, p, QN, qn, qv, q, RN, rn, rv, r, X)
#define StructPQRX(B, PN, pn, pv, p, QN, qn, qv, q, RN, rn, rv, r, X) \
StructPQRXNF(B, PN, pn, pv, p, QN, qn, qv, q, RN, rn, rv, r, X) \
struct FV##B##PN##QN##RN##X : DP::Factory<V##B##PN##QN##RN##X, int, pn, qn, rn> {}; \
struct FA##B##PN##QN##RN##X : DP::Factory<A##B##PN##QN##RN##X, int, pn, qn, rn> {};

Struct(Base)
StructX(Base, DC)
StructX(Base, NC)
StructP(Base, Int, int, int const&, i)
StructPX(Base, Int, int, int const&, i, DC)
StructPX(Base, Int, int, int const&, i, NC)
StructP(Base, String, std::string, std::string const&, s)
StructPX(Base, String, std::string, std::string const&, s, DC)
StructPX(Base, String, std::string, std::string const&, s, NC)
StructPQ(Base, Int, int, int const&, i, String, std::string, std::string const&, s)
StructPQX(Base, Int, int, int const&, i, String, std::string, std::string const&, s, DC)
StructPQX(Base, Int, int, int const&, i, String, std::string, std::string const&, s, NC)
StructPXNF(Base, Stream, Stream::Input&, Stream::Input&, stream, DM)
StructPXNF(Base, Stream, Stream::Input&, Stream::Input&, stream, NC)
StructPQXNF(Base, Stream, Stream::Input&, Stream::Input&, stream, Int, int, int const&, i, DM)
StructPQXNF(Base, Stream, Stream::Input&, Stream::Input&, stream, Int, int, int const&, i, NC)
StructPQRXNF(Base, Stream, Stream::Input&, Stream::Input&, stream, Int, int, int const&, i, String, std::string, std::string const&, s, DM)
StructPQRXNF(Base, Stream, Stream::Input&, Stream::Input&, stream, Int, int, int const&, i, String, std::string, std::string const&, s, NC)

template <std::size_t M, std::size_t N>
int
TestBalance(DS::TNode<M> const* t, int depth = 0) {
	if (t) {
		++depth;
		int leftDepth = t->d[N].hasLeft ? TestBalance<M, N>(t->template left<N>(), depth) : depth;
		int rightDepth = t->d[N].hasRight ? TestBalance<M, N>(t->template right<N>(), depth) : depth;

		if (leftDepth > rightDepth) {
			assert(!t->d[N].isRight);
			assert(!t->d[N].isBalanced);
			assert(t->d[N].isLeft);
			assert((leftDepth - rightDepth < 2));
			return leftDepth;
		}
		if (rightDepth > leftDepth) {
			assert(t->d[N].isRight);
			assert(!t->d[N].isBalanced);
			assert(!t->d[N].isLeft);
			assert((rightDepth - leftDepth < 2));
			return rightDepth;
		}
		assert(!t->d[N].isRight);
		assert(t->d[N].isBalanced);
		assert(!t->d[N].isLeft);
		return leftDepth;
	}
	return 0;
}

}//namespace DSTest

#endif //DSTEST_UTIL
