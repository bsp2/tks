
.PHONY: all
all: test_host.tkx test_client.tkx test_client_benchmark.tkx

test_host.tkx: test_host.tks test_host.tkp ../rs.tks
	tks -c test_host

test_client.tkx: test_client.tks test_client.tkp ../rs.tks
	tks -c test_client

test_client_benchmark.tkx: test_client_benchmark.tks test_client_benchmark.tkp ../rs.tks
	tks -c test_client_benchmark

.PHONY: install
install: test_host.tkx test_client.tkx test_client_benchmark.tkx
	cp test_host.tkx "$(TKS_SITE_PREFIX)/applications/"
	cp test_client.tkx "$(TKS_SITE_PREFIX)/applications/"
	cp test_client_benchmark.tkx "$(TKS_SITE_PREFIX)/applications/"



