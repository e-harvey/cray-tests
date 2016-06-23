AM_CFLAGS = -I$(srcdir)/include -I$(srcdir)/common
ACLOCAL_AMFLAGS = -I config

bin_PROGRAMS = \
	performance/single-node/lock_tests

# dist_bin_SCRIPTS = \
# 	scripts/dummy.sh

#performance_single_node_lock_tests_SOURCES = \
#	performance/single-node/lock_test.c

EXTRA_DIST = \
	ft_utils.h \
	ft_tbarrier.h

if HAVE_PMI
AM_CFLAGS += -lpmi

if HAVE_CRAY_PMI
AM_CFLAGS += -DCRAY_PMI_COLL
endif

endif

test:
	./scripts/dummy.sh
