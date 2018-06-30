#pragma once

#ifndef CONCAT
#define CONCAT_INNER(a,b) a##b
#define CONCAT(a,b) CONCAT_INNER(a,b)
#endif

#define ON_LEAVE(statement) \
	struct CONCAT(s_ol_, __LINE__) { \
		~CONCAT(s_ol_, __LINE__)() { statement; } \
	} CONCAT(v_ol_, __LINE__);

#define ON_LEAVE_1(statement, type, var) \
	struct CONCAT(s_ol_, __LINE__) { \
		type var; \
		CONCAT(s_ol_, __LINE__)(type v): var(v) {} \
		~CONCAT(s_ol_, __LINE__)() { statement; } \
	} CONCAT(v_ol_, __LINE__)(var);

#define ON_LEAVE_2(statement, type1, var1, type2, var2) \
	struct CONCAT(s_ol_, __LINE__) { \
		type1 var1; type2 var2; \
		CONCAT(s_ol_, __LINE__)(type1 v1, type2 v2): var1(v1), var2(v2) {} \
		~CONCAT(s_ol_, __LINE__)() { statement; } \
	} CONCAT(v_ol_, __LINE__)(var1, var2);

#define ON_LEAVE_3(statement, type1, var1, type2, var2, type3, var3) \
	struct CONCAT(s_ol_, __LINE__) { \
		type1 var1; type2 var2; type3 var3; \
		CONCAT(s_ol_, __LINE__)(type1 v1, type2 v2, type3 v3): var1(v1), var2(v2), var3(v3) {} \
		~CONCAT(s_ol_, __LINE__)() { statement; } \
	} CONCAT(v_ol_, __LINE__)(var1, var2, var3);

#define ON_LEAVE_4(statement, type1, var1, type2, var2, type3, var3, type4, var4) \
	struct CONCAT(s_ol_, __LINE__) { \
		type1 var1; type2 var2; type3 var3; type4 var4; \
		CONCAT(s_ol_, __LINE__)(type1 v1, type2 v2, type3 v3, type4 v4): var1(v1), var2(v2), var3(v3), var4(v4) {} \
		~CONCAT(s_ol_, __LINE__)() { statement; } \
	} CONCAT(v_ol_, __LINE__)(var1, var2, var3, var4);

