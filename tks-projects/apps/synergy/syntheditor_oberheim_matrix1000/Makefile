
ALL_SRC= \
	Matrix1000.tks \
	Matrix1000.xfm

syntheditor_oberheim_matrix1000.tsl: $(ALL_SRC)
	tks -cl syntheditor_oberheim_matrix1000 && mv syntheditor_oberheim_matrix1000.tsl ../libraries/

.PHONY: clean
clean:
	rm -f syntheditor_oberheim_matrix1000.tsl
