% annotated relations
% smmx_to_markdown
% 12-Jan-2020

$(var:header)

# Table of Contents
$(buf!toc)



annotated relations
===================


## bear
   - *is a* [mammal](#mammal), *eats* [fish](#fish)


### mammal


### fish


table
=====
   - *see* [wikipedia](#wikipedia)
   - **to**: *is* [furniture](#furniture)


## wikipedia
**Link**: [https://en.wikipedia.org/wiki/Table_(furniture)](https://en.wikipedia.org/wiki/Table_%28furniture%29) *(url)*



furniture
=========
   - **from**: [table](#table)
   - **to**: *made of* [wood](#wood)


wood
====
   - **from**: [furniture](#furniture)


$(buf!footnote)
$(buf!w3validator)

Document created on $(var:localdatetime)
].create
