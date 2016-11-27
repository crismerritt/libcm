CC = gcc
CFLAGS = -g
CPPFLAGS = -I.
OBJS = cm_list.o cm_table.o cm_set.o cm_heap.o 
TEST_OBJS = Test.o
ALL_OBJS = $(OBJS) $(TEST_OBJS)
LIBCM = libcm.a
#PKGDEST=/net/ttssun02/export/release/incoming
#PKGDEST=/install/packages
PKG=cmlibcm
PKGDEST=/var/spool/pkg

pkg: $(LIBCM)
	umask 002 ; pkgmk -o -d ${PKGDEST} MICOVERSION=${MICOVERSION} ; find ${PKGDEST}/${PKG} -type d -exec chmod 775 {} \;

release: check_for_mods pkg
	@tag=`awk -F= '/^PSTAMP/ { print $$2 }' ${PKGDEST}/${PKG}/pkginfo` \
	&& echo "Tagging release with PSTAMP $$tag" \
	&& cvs -q tag $$tag .

check_for_mods:
	@if ( cvs -nq update | grep '^[AM]' ) \
	then \
		echo "Check in modifications before making a release"; \
		exit 1; \
	fi

$(LIBCM): $(OBJS)
	ar r $@ $(OBJS)

Test: Test.o $(LIBCM)
	$(CC) $(CFLAGS) Test.o $(LIBCM) -o $@

cm_list.o: cm_list.c cm_list.h
cm_table.o: cm_table.c cm_table.h
cm_heap.o: cm_heap.c cm_heap.h
Test.o: Test.c cm_list.h cm_table.h cm_heap.h

clean:
	rm -f $(LIBCM) $(ALL_OBJS) Test core *~ #*#

