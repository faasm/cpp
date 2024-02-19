.data
.global .gomp_critical_user_
.global .gomp_critical_user_.var
.global .gomp_critical_user_.reduction.var
.global __kmp_unnamed_critical_addr
.gomp_critical_user_:
.zero 4
.size .gomp_critical_user_, 4
.gomp_critical_user_.var:
.zero 4
.size .gomp_critical_user_.var, 4
.gomp_critical_user_.reduction.var:
.zero 4
.size .gomp_critical_user_.reduction.var, 4
__kmp_unnamed_critical_addr:
    .4byte .gomp_critical_user_
    .size __kmp_unnamed_critical_addr, 4
