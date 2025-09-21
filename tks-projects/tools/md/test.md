% Markdown Test Document
% bsp
% 18-Dec-2024
>>>
26Sep2018..06Oct2018, 04Nov2019, 23Jan2020, 02Feb2020, 18Dec2024
<<<

$(var:header)

# Introduction

`md` is an extended re-implementation of the [^markdown][] utility script, originally released in 2004 by John Gruber.

`md` implements most of the markdown syntax, with some notable changes / omissions:

  - Four spaces at the beginning of a line do **not** produce a code block
  - A newline in a text paragraph inserts a line break
     - Similar to [^asciidoc][], for example
  - Inline HTML is not supported
     - I think this defeats the purpose of an output-format agnostic markup language.
  - Lists may not contain block quotes
     - but they can contain buffer refs (which can contain arbitrary markdown)

`md` also adds a couple of **extensions**:

  - Support for [^pandoc][] style document headers (\% title, \% author(s), \% date)
  - Support for explicit header anchor ids (`\# Header Title \{my-id\}`)
  - Support for fenced code blocks (`\~\~\~`)
  - Nested lists
     - Sub-lists are started when their indent level is higher than the previous' list indentation
        - Consequently, the minimum indent to start a sub-list is one char, not three like in [^pandoc][].
  - Ordered list sequence start and explicit list item numbers
     - Ordered lists can start at an offset, and list item sequence numbers can be skipped
  - Tables
     - Configurable table cell alignment
     - Configurable table cell widths
     - Multi-line table cells
  - Variables
     - Preprocessor variables can be set in the markdown source or on the command line
     - Variable contents can be declared *inline* or be read from external files
  - Post processor includes (raw HTML)
  - Buffer redirection
     - The HTML output of sections of the markdown document can be redirected to named memory buffers
     - Memory buffers can be inserted at any point in the document
        - This makes it possible to create complex document structures, e.g. tables within tables
  - Image scaling
     - Override width, height, or both
     - Scaled images will link to the original-size image
  - Sub-titled paragraphs
     - Can be used to add a sub-title to e.g. an image or a table
  - Redefinable link references
     - Link targets can be redefined
        - Previous (forward) references will be resolved as soon as a link reference definition becomes available
  - Admonition paragraphs (`NOTE:`, `IMPORTANT:`, `TIP:`, `CAUTION:`, `WARNING:`)
     - Similar to [^asciidoc][], for example
     - Single or multi-line (multi-paragraph)
     - Small or large (e.g. `note:`, `NOTE:`, ...)
  - Footnotes
     - Local footnotes tables can be inserted right after the footnote definitions
     - Global footnotes typically occur at the end of the document but it's possible to split these into multiple tables
  - Template mode
     - Load (complex) HTML template and insert buffer contents.
     - Document body will be stored in `body` buffer
     - Table of contents will be stored in `toc` buffer
     - Global footnotes will be stored in `footnote` buffer
     - Additional content can be stored in user defined buffers
  - Custom CSS classes
     - Each paragraph can be assigned an additional, custom CSS class
  - Special char entities ((C), (R), <-, <=, =>, ->, ..)
  - Multi-column print layout (`+++col`, `+++/col`)
  - Citations
  - Single-file, inline-data mode (`-il`, `--inline-data`)
     - Inline additional content (CSS, images) via data URIs
     - The resulting self-contained document can be archived or sent via email

[^markdown]: https://daringfireball.net/projects/markdown/ "a text-to-HTML conversion tool for web writers"
[^asciidoc]: http://asciidoc.org/ "a text document format for writing notes, documentation, articles, books, ebooks, slideshows, web pages, man pages and blogs"
  [^pandoc]: https://pandoc.org/ "a universal document converter"



# Table of Contents
$(buf!toc)




# Command line options

`md` supports the following command line options:

>>>partialnote
note: This automatically enables the *partial* processing mode.
<<<

|noheader
|cols=5,20
|:-o *outputfile*    |:Set output pathname.
|                    |+The default output name is derived from the input file (replace suffix by `.html`)
| -stdout            | Write generated HTML output to stdout
| -stdin             | Read markdown input from stdin (the input filename will only be used to find include files)
| -css *cssurl*      | Set CSS URL.
|                    |+The default CSS name is derived from the input file (replace suffix by `.css`)
| -D*var*=*content*  | Set variable (see [#Variables][])
| -D*var*='*<file*'  | Set variable to content of *file*
| -B*buf*=*content*  | Set buffer (see [Buffers][#buffer-redirection])
| -B*buf*='*<file*'  | Set buffer to content of *file*
| -p                 | Enable partial document generation (skip `\<html>` and `\<body>`)
|+-\-partial         |+ 
| -cp *class_prefix* | Set CSS class prefix
| -ip *icon_prefix*  | Set icon resource prefix (default is `icons/`)

| -x *template_file* | Use HTML template file.
|+                   |+The generated document body will be placed in buffer `body` and can be inserted at any point in the template via `\$\(buf\!body\)`.
|+                   |+$(buf!partialnote)
| -ae                | Use plain ASCII for special character entities (unicode may confuse html-to-pdf tools)
|+--ascii-entities   |+ 
| -ne                | Disable header enumeration
|+--no-enumeration   |+ 
| -eo                | Set header enumeration offset (1..n)
|+--enumeration-offset *nr* |+ 
| -il                | Inline external files via data URIs and create self-contained document.
|+--inline-data      |+
| -h                 | Show usage
|+--help             |+ 
].cmdline

+++
## Examples
~~~
$ tks app:md myfile.md
~~~
] Example 1
Markdown input will be read from `myfile.md` and HTML output will be written to `myfile.html`, using the `myfile.css` stylesheet.
  
  

~~~
$ tks app:md -o myoutput.html -css mystylesheet.css input.md
~~~
] Example 2
Markdown input will be read from `myfile.md` and HTML output will be written to `myoutput.html`, using the `mystylesheet.css` stylesheet.
  
  

~~~
$ tks app:md -x mytemplate.html myfile.md
~~~
] Example 3
Markdown input will be read from `myfile.md` and HTML output will be written to `myfile.html`, using the `mytemplate.html` template file and the `myfile.css` stylesheet.



# Plain text paragraphs

Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua.
At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.

Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua.

NOTE:
Paragraphs are separated by empty lines.

A line break is inserted when a non-blank line is followed by another non-blank line.
====

# Multi-column layout (print)
The `+++col` and `+++/col` char sequences are used to enable / disable the multi-column layout mode.

+++col
Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua.
At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.

Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua.
Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua.
At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.

Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua.

Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua.

At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.
* Lorem ipsum dolor sit amet
* Consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua.

Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua.
At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.
Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua.
+++/col

CAUTION:
The multi-column layout will only appear in the print version of the document.
====


# Text styles

*Some italic text*^superscript text^

Plain text *Some emphasized text* more plain text _more emphasized text_^superscript text^ remaining plain text.

Some ~subscript~ text. **strong text** or __strong text__. ~~strikethrough~~, `fixed`, ``more fixed text``

A new paragraph with more regular text.

## Text style table

  |:Example                  |:Output
  | `\*Emphasis\*`           | *Emphasis*
  | `\_Emphasis2\_`          | _Emphasis2_
  | `\*\*Strong\*\*`         | **Strong**
  | `\_\_Strong\_\_`         | __Strong2__
  | `\~\~Strike-through\~\~` | ~~Strike-through~~
  | `\`Fixed\``              | `Fixed`
  | `\`\`Fixed\`\``          | ``Fixed2``
  | `\~Subscript\~`          | ~Subscript~
  | `\^Superscript\^`        | ^Superscript^
  ] Text styles




# Escaped chars

Escaped control chars: \\ backslash, \` backtick, \* asterisk, \_ underscore, \{\} curly braces, \[\] square braces, \(\) parenthesis, \# hash, \+ plus, \- minus, \. dot, \! exclamation mark, \~ tilde, \| pipe, \< less than, \> greater than.

Unescaped control chars like * asterisk or _ underscore. Freestanding chars like * will be treated as plain text.

Some escaped \* asterisk. **strong text with escaped asterisk\***  ~subscript with escaped \~ tilde~.



# Forced line breaks

## Empty line with two spaces (<br>)

Forced line breaks are inserted when an empty line starts with two spaces:
Line 1
  
  
  
Line 5.



# Forced page breaks (print)

A page break in the print version of the document can be inserted via
~~~
\+\+\+
~~~

TIP:
The default CSS automatically inserts a page break before each 1st level header.
===

CAUTION:
Page breaks will only appear in the print version of the document.
====



## Linefeed paragraph via backslash

Under certain circumstances, for example when using custom CSS classes for advanced layouts, it may be necessary to force the end of the current paragraph.

A line that only consists of the `\\` character will insert a linefeed div (CSS class `.lf`):
~~~
line 1
\
\
\
line 2
~~~
=>
line 1
\
\
\
line 2




# Special char entities

%<3% hello, world %:)%

| Code       | Output
| `\(C)`     | (C)
| `\(R)`     | (R)
| `\(TM)`    | (TM)
| `\--`      | --
| `\...`     | ...
| `\->`      | ->
| `\=>`      | =>
| `\<-`      | <-
| `\<=`      | <=
| `\<=\>`    | <=>
| `\<\!=\>`  | <!=>
] Special char entities

| Code       | Output
| `%:\)%`    | %:)%
| `%;\)%`    | %;)%
| `\%:\|%`   | %:\|%
| `%;\(%`    | %;(%
| `\%:D%`    | %:D%
| `\%:P%`    | %:P%
| `\%:O%`    | %:O%
| `\%>(%`    | %>(%
| `\%:'(%`   | %:'(%
| `\%8)%`    | %8)%
| `\%.)%`    | %.)%
] Smileys

| Code       | Output
| `\%\<3%`   | %<3%
| `\%\*%`    | %*%
] Other entities



# Variables

`md`\`s pre-processor can be used to set variables, and insert them at any point in the document.

Variables are set via the `\$\(var:<varname>=<value>)` syntax.
Variable contents are inserted via the `\$\(var:<varname>)` syntax.

## Set variable to inlined content

Set test to 42:
~~~
$\(var:test=42)
~~~
$(var:test=42)
Variable `test` now contains >$(var:test)<.

~~~
Set variable `test` to 3.1415 in one line: $\(var:test=3.1415)>$\(var:test)<
~~~
Set variable `test` to 3.1415 in one line: $(var:test=3.1415)>$(var:test)<

Set variable `test` to "hello, world.": $(var:test=hello, world.) Var content: >$(var:test)<

## Set variable to file content

~~~
Set variable `test` to file content: $\(var:test=<res/var_content.txt). Var content: >$\(var:test)<
~~~
Set variable `test` to file content: $(var:test=<res/var_content.txt). Var content: >$(var:test)<


## Externally defined vars

Variables can also be defined via the `-D` command line option
- either directly, e.g. `-Dmyextvar="my externally defined var"`
- or via file content, e.g. `-Dmyextvar='\<var_content.txt'`

`myextvar` is currently defined as >$(var:myextvar)<

+++
## Built-in special variables

The following variables have special meaning:

>>>headersrc
~~~
|noheader
| Document\: :|:$\(buf!title)
| Author\:   :|:$\(buf!author)
| Date\:     :|:$\(buf!date)
].header
~~~
<<<

|:Name                      |:Description
| `header`                  | A built-in markdown snippet that creates a simple document header\:
|                           |+$(buf!headersrc)
| `localdate`               | Current date, e.g. `$(var:localdate)`
| `localtime`               | Current time, e.g. `$(var:localtime)"
| `localdatetime`           | Current date and time, e.g. `$(var:localdatetime)`
| `localyear`               | Current year, e.g. `$(var:localyear)`
| `localmonth`              | Current month, e.g. `$(var:localmonth)`
| `localmonthnameshort`     | Current month name, e.g. `$(var:localmonthnameshort)`
| `localmonthnamelong`      | Current month name, e.g. `$(var:localmonthnamelong)`
| `localdayofmonth`         | Current day of month, e.g. `$(var:localdayofmonth)`
| `localdayofmonthname`     | Current day of month, e.g. `$(var:localdayofmonthname)`
| `localdayofweeknameshort` | Current day of week, e.g. `$(var:localdayofweeknameshort)`
| `localdayofweeknamelong`  | Current day of week, e.g. `$(var:localdayofweeknamelong)`
] Special variables


# Include markdown

## Example
~~~
Markdown content can be included from an external file: >$\(inc:res/var_content_3.md)<
~~~
Markdown content can be included from an external file: >$(inc:res/var_content_3.md)<

TIP:
Alternatively, the value of a variable can be set to the content of a file via the `-Dmyvar='\<myfile.md'` cmdline option, then the variable can be inserted at any point via `\$\(var:myvar\)`.
===

## Parametrization

Along with [#variables][] and / or [redefined link refs](#links-redefined-refs), this preprocessor feature can be used to parametrize the included markdown content by 
- redefining variables before the include
- redefining referenced links before or directly after the include



# Include HTML

In the postprocessor pass, additional, pre-generated HTML content can be included from files or memory buffers.

## Example
~~~
Raw HTML content can be included from an external file: >$\(inc!res/html_content.html)<
~~~

Raw HTML content can be included from an external file: >$(inc!res/html_content.html)<

NOTE: Postprocessor instructions use the `\$\(\<cmd\>!\<arg\>)` syntax.

NOTE: The reason for using a different syntax (`!` instead of `:`) is that this way the preprocessor can generate postprocessor instructions.

## HTML preprocessor include file parametrization
Included HTML files can be parametrized by including them via the `\$\(inc?html_macro.html\)` syntax.
`<var:myvarname>` occurences in the included HTML file will be replaced by the current variable content(s), i.e. the variable replacement and HTML file inclusion is performed during the preprocessor pass.

A buffer cmd (`\$\(buf!htmlprebuf<id>)`) will then be inserted (internally) into the markdown data and will later (during post processing) be expanded as usual.

This can be used for creating thumb nail images that link to full size images, for example.



# Buffer redirection

Sections of the document can be redirected to memory buffers by enclosing them in `\>\>\>` and `\<\<\<`:
~~~
>>>buf_id
..buffer content..
<<<
~~~
These buffers can then be inserted at any point in the generated document via the `\$\(buf\!buf_id\)` post-processor command.

TIP:
When creating multiple buffers, the `\<\<\<` sequence before the next buffer can be omitted:
~~~
\>>>buf1
buf1 content
\>>>buf2
buf2 content
\<<<
~~~
===

## Example
>>>mybuf
This is the content of buffer `mybuf`.

It may contain arbitrary markdown text, except for `\>\>\>` and `\<\<\<`.

Here's a code block:
~~~
some code
~~~

And a table:
  |a |b
  |a1|b1

<<<

~~~
- A list that contains a nested buffer
   - $\(buf!mybuf)
- Another list item
~~~
results in:
- A list that contains a nested buffer
   - $(buf!mybuf)
- Another list item
] A list with a nested buffer


+++
## Special buffer names
The following buffer names have special meanings:
  |:Buffer        |:Description
  | `body`        | Stores the main document body in template mode.
  | `toc`         | Stores the table of contents
  | `footnote`    | Stores the current (global) footnote table
  | `w3validator` | Stores a link to the [validator.w3.org](http://http://validator.w3.org/check) service.
  | `title`       | Document title (set via `%` header)
  | `author`      | Document author(s) (set via `%` header)
  | `date`        | Document title (set via `%` header)
  ] Special buffers

TIP:
When the tool is run in template mode (`-x`), the output buffer is created by loading the provided template file and post processing it.

The generated markdown content can be inserted at any point in the template by inserting a buffer (e.g. `\$\(buf!body)`), or by including an external file (e.g. `\$\(inc!myfile.html)`).
===




# Pre-formatted text (code block)
Code blocks are created by enclosing a section of the document in `\~\~\~`:
~~~
\~\~\~
..fenced code block contents..
\~\~\~
~~~
=>
~~~
..fenced code block contents..
~~~ 

## Indented code blocks

Unlike the [original markdown](https://daringfireball.net/projects/markdown/) implementation, four (or more) spaces at the beginning of a line do **not** produce pre-formatted code blocks.

Code blocks can be indented, and the indentation will not be visible in the output file:
~~~
     \~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     another code block.
      (indented in the markdown source file)
       - raw text list item 1
          - raw text list 1.1
     \~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     ] Indented code block example
~~~
=>
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     another code block.
      (indented in the markdown source file)
       - raw text list item 1
          - raw text list 1.1
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     ] Indented code block example

Some more text.



# Block quotes
Block quotes are created by prepending a line with one or more `\> ` strings (the number of strings determines the block level).

~~~
\> a block quote
~~~
=>
> a block quote

~~~
\> > a double block quote
~~~
=>
> > a double block quote

IMPORTANT:
The `>` characters must be separated by spaces to distinguish them from the buffer redirection char sequence (`>>>`).
=========
~~~
\> a block quote
\> > directly followed by a double quote
\> and back to a single quote
~~~
=>
> a block quote
> > directly followed by a double quote
> and back to a single quote

Block quotes can be used with all paragraph types, e.g.
> |a|b
> |a1|b1
or
> - list item 1
> - list item 2
> - list item 3
or
> A block quote that contains a fenced code block:
> > ~~~
> > ..code block content..
> > ~~~
> some text

+++
## Citations
~~~
\> The secret of getting ahead is getting started. The secret of getting started is breaking your complex overwhelming tasks into small manageable tasks, and then starting on the first one.
\> -- Mark Twain[-][^marktwain]
~~~
=>
> The secret of getting ahead is getting started. The secret of getting started is breaking your complex overwhelming tasks into small manageable tasks, and then starting on the first one.
> -- Mark Twain[-][^marktwain]

[^marktwain]: - "Maybe he was not the first one to say this ;)"

### Nested citations

> A block quote
> > with a citation
> > -- me
> back to 1st level quote



# Admonition paragraphs

## NOTE:
~~~
NOTE: This is a note.
~~~
=>
NOTE: This is a note.

~~~
note: This is a small note.
~~~
=>
note: This is a small note.
  

## IMPORTANT:
~~~
IMPORTANT: This is an important note.
~~~
=>
IMPORTANT: This is an important note.

~~~
important: This is a small, important note.
~~~
=>
important: This is a small, important note.

  
## TIP:
~~~
TIP: This is a tip.
~~~
=>
TIP: This is a tip.

~~~
tip: This is a small tip.
~~~
=>
tip: This is a small tip.

  
## CAUTION:
~~~
CAUTION: Take caution !
~~~
=>
CAUTION: Take caution !

~~~
caution: Take (a bit of) caution !
~~~
=>
caution: Take (a bit of) caution !
  

## WARNING:
~~~
WARNING: This is a warning.
~~~
=>
WARNING: This is a warning.

~~~
warning: This is a small warning.
~~~
=>
warning: This is a small warning.


## TODO:
~~~
TODO: This is a to-do.
~~~
=>
TODO: This is a to-do.

~~~
todo: This is a small to-do.
~~~
=>
todo: This is a small to-do.


+++
## Multi-paragraph admonitions
~~~
TIP:
Admonitions can span multiple lines.
They may contain arbitrary markdown, except for further admonitions.

Lists:
- list item 1
- list item 2
- list item 3

Tables:
|a|b
|a1|b1

\> block quote
===
~~~
=>
TIP:
Admonitions can span multiple lines.
They may contain arbitrary markdown, except for further admonitions.

Lists:
- list item 1
- list item 2
- list item 3

Tables:
|a|b
|a1|b1

> block quote
===

+++
## Admonition icon resources
By default, admonition icons will link to the following files:
|:Type         |:File
| `note:`      | `icons/note_s.png`
| `NOTE:`      | `icons/note_l.png`
| `important:` | `icons/important_s.png`
| `IMPORTANT:` | `icons/important_l.png`
| `tip:`       | `icons/tip_s.png`
| `TIP:`       | `icons/tip_l.png`
| `caution:`   | `icons/caution_s.png`
| `CAUTION:`   | `icons/caution_l.png`
| `warning:`   | `icons/warning_s.png`
| `WARNING:`   | `icons/warning_l.png`
| `todo:`      | `icons/todo_s.png`
| `TODO:`      | `icons/todo_l.png`
] Icon resources

## Alternative icon resource path
The default icon path prefix (`icons/`) can be changed via the `-ip` [command line switch](#command-line-options).

## Inlined icon resources
When using the `-il`, `--inline-data` command line option, all external images, and the CSS file, will be embedded in the output document.

While this may increase the file size considerably (depending on the number of images and their sizes), this makes it possible to easily distribute the self-contained document via email, similar to a PDF.



# Header 1 <test>

The syntax for *atx*-style headers is:
~~~
# myheader
~~~
The number of `#` characters determines the header level (1..9).

To exclude a sub-header from the table of contents (TOC), add a `-` character as follows:
~~~
#-# myheader (excluded from toc)
~~~


## Header 2

### Header 3

#### Header 4

##### Header 5

###### Header 6

####### Header 7

######## Header 8

######### Header 9

Header 1 via =====
==================

The syntax for *setext*-style level 1 headers is:
~~~
My Header
=========
~~~


Header 2 via \-\-\-
------------------

The syntax for *setext*-style level 2 headers is:
~~~
My Header
---------
~~~

# Explicit header anchors
As an extension to [^markdown][], it is possible to specify explicit anchor ids.

This can be necessary when..
  - the auto-generated anchors (derived from the header caption) are not unique
  - the header caption is rather long and / or it becomes difficult to guess the auto-generated anchor id
  - the markdown itself is auto-generated from a different document tree (e.g. `org` work breakdown structures with explicit task ids)

Example:
~~~~~
# Test {module1-test}

# Test {module2-test}

# A rather long title (..can you guess the anchor id??!) {long-title}
~~~~~
] explicit_header_anchors.md



# Horizontal rules

## Dash
~~~
---
~~~
=>
---


## Asterisk
~~~
***
~~~
=>
***


## Asterisk (book)
~~~
* * *
~~~
=>
* * *


## Pagebreak (print)
~~~
+++
~~~
=>
+++



# Lists
~~~
- list item 1
- list item 2
- list item 3
~~~
=>
- list item 1
- list item 2
- list item 3

~~~
* list item 1
* list item 2
* list item 3
~~~
=>
* list item 1
* list item 2
* list item 3

~~~
1) list item 1
2) list item 2
3) list item 3
~~~
=>
1) list item 1
2) list item 2
3) list item 3

~~~
1. list item 1
2. list item 2
3. list item 3
~~~
=>
1. list item 1
2. list item 2
3. list item 3


+++
## Unordered, nested lists

An unordered, nested list:
~~~
   - list item 1
      - sublist item 1.1
         - sublist item 1.1.1
            - sublist item 1.1.1.1
              List items can span multiple lines (when indented).

              This includes newlines.
              Some *formatted* text.
         - sublist item 1.1.2
      - sublist item 1.2
   - list item 2
   - list item 3
~~~
=>
   - list item 1
      - sublist item 1.1
         - sublist item 1.1.1
            - sublist item 1.1.1.1
              List items can span multiple lines (when indented).

              This includes newlines.
              Some *formatted* text.
         - sublist item 1.1.2
      - sublist item 1.2
   - list item 2
   - list item 3


+++
## Ordered, nested lists

An ordered list:
~~~
   1) list item 1
      1. sublist item 1.1
         1. sublist item 1.1.1
            1) sublist item 1.1.1.1
         2. sublist item 1.1.2
      2. sublist item 1.2
   2) list item 2
   3) list item 3
~~~
=>
   1) list item 1
      1. sublist item 1.1
         1. sublist item 1.1.1
            1) sublist item 1.1.1.1
         2. sublist item 1.1.2
      2. sublist item 1.2
   2) list item 2
   3) list item 3


### Start offset

Ordered lists can start at an offset:

   4) list item 4
   5) list item 5


### Skip

It's also possible to skip sequence numbers:

   6) list item 6
   8) list item 8 (*skip 7*)
  10) list item 10 (*skip 9*)
  12) list item 12 (*skip 11*)
  14) list item 14 (*skip 13*)

+++
### Auto-enumeration
~~~
. one
. two
. three
~~~
=>
. one
. two
. three


## List sub-titles

An unordered list with a sub title:
   - list item 1
   - list item 2
   - list item 3
   ] Unordered list sub title





# Links

## Direct Links
~~~
See \[this example page\](http://www.example.com/doc3.html)
~~~
See [this example page](http://www.example.com/doc3.html).


## Direct Links with description
~~~
Also see \[this other example page](http://www.example.com/doc4.html "doc4 link description").
~~~
Also see [this other example page](http://www.example.com/doc4.html "doc4 link description").

NOTE: The link description is shown as a tooltip when hovering over the link.


## Document internal links
~~~
A \[document internal link](#header-1-test).
~~~
A [document internal link](#header-1-test).

~~~
A link to the \[#introduction][].
~~~
A link to the [#introduction][].


## Automatic Links
~~~
See \<http//www.example.com/doc5.html\>.
~~~
See <http//www.example.com/doc5.html>.

Write an email to <info@example.com> (=> will be obfuscated (entity encoding)+ `mailto:` will be auto-preprended.).

An (obfuscated) FTP link: <ftp://myuser:mypasswd@ftp.example.com>.


+++
## Link refs

Link declaration:
~~~
\[example.com]: http://www.example.com
~~~
[example.com]: http://www.example.com

Link reference:
~~~
A link to \[example.com]\[].
~~~
=>
A link to [example.com][].

~~~
A link to \[another example site]\[example].
\[example]: http://www.example.com
~~~
=>
A link to [another example site][example].
[example]: http://www.example.com


### Links (redefined refs)

Referenced link targets can be redefined:

~~~
See \[this]\[1] or \[that]\[2] document.

\[1]: http://www.example.com/doc1a.html
\[2]: http://www.example.com/doc1b.html
~~~
=>
See [this][1] or [that][2] document.

[1]: http://www.example.com/doc1a.html
[2]: http://www.example.com/doc1b.html


~~~
Also see \[here]\[1] or \[there]\[2].

\[1]: http://www.example.com/doc2a.html
\[2]: http://www.example.com/doc2b.html
~~~
=>
Also see [here][1] or [there][2].

[1]: http://www.example.com/doc2a.html
[2]: http://www.example.com/doc2b.html




# Footnotes

## Local footnotes
~~~
See \[this]\[^1] or \[that]\[^2] document.

\[^1]: http://www.example.com/doc3a.html "Document 3a"
\[^2]: http://www.example.com/doc3b.html "Document 3b"
\[^^]
~~~
=>
See [this][^1] or [that][^2] document.

[^1]: http://www.example.com/doc3a.html "Document 3a"
[^2]: http://www.example.com/doc3b.html "Document 3b"
[^^]

NOTE:
The local footnote table is inserted when the list of footnote definitions is followed by `\[\^\^\]`.
~~~
\[^1]: http://www.example.com/doc3a.html "Document 3a"
\[^2]: http://www.example.com/doc3b.html "Document 3b"
\[^^]
~~~
This can be used to insert a footnote table at the end of each logical document section, in addition to an optional, global footnote table at the end of the document.
===

~~~
Also see \[^here]\[\] or \[^there]\[\].

 \[^here]: http://www.example.com/doc4a.html
\[^there]: http://www.example.com/doc4b.html
\[^^]
~~~
=>
Also see [^here][] or [^there][].

 [^here]: http://www.example.com/doc4a.html
[^there]: http://www.example.com/doc4b.html
[^^]


+++
## Global footnotes

The syntax for creating a global footnote is the same as for local footnotes.

A footnote becomes global when it has not been consumed in a [local footnote table][^localfootnote].

[^localfootnote]: #local-footnotes "a link to the local footnote documentation (global footnote test)"

NOTE:
The global footnote table is created by inserting the `footnote` buffer at any point in the document:
~~~
$\(buf!footnote)
~~~
While the buffer can be generated / inserted multiple times, once inserted, a footnote will be marked as *done*.
Each global footnote table will only contain footnotes that have not been displayed in previous tables.
===



## Footnote-only links

On rare occasions it can be useful to link only to the footnote, e.g.
~~~
The sky is blue[-][^bluesky].
~~~
=>
The sky is blue[-][^bluesky].

[^bluesky]: https://en.wikipedia.org/wiki/Wikipedia:You_do_need_to_cite_that_the_sky_is_blue "(yep, it's due to [Rayleigh scattering](https://en.wikipedia.org/wiki/Rayleigh_scattering))"


# Images
## Direct image links
~~~
Here's an image: \!\[kitty cat (alt text)](res/image.png "kitty cat (title\)")
~~~
Here's an image: ![kitty cat (alt text)](res/image.png "kitty cat (title\)").

~~~
Here's an image without alt text and title: \!\[](res/image.png)
~~~
Here's an image without alt text and title: ![](res/image.png).

## Scaled image links
~~~
Here's a scaled image: \!\[kitty cat (alt text)](res/image.png "kitty cat (title\)" w=50).
~~~
Here's a scaled image: ![kitty cat (alt text)](res/image.png "kitty cat (title\)" w=50).

## Referenced images
~~~
Here's a referenced image: \!\[kitty cat (alt text)]\[kitty].
~~~
Here's a referenced image: ![kitty cat (alt text)][kitty].

~~~
Here's a referenced, scaled image: \!\[kitty cat (alt text)]\[kitty_scl].
~~~
Here's a referenced, scaled image: ![kitty cat (alt text)][kitty_scl].

NOTE: Scaled images will link to the original-size image resources.

+++
## Sub-titled images
   ~~~
   \!\[kitty]\[]
   \] Sub-titled image
   ~~~
=>
\
   ![kitty][]
   ] Sub-titled image

[kitty]: res/image.png "kitty cat (title)"
[kitty_scl]: res/image.png "scaled kitty cat (title)" h=30

NOTE:
Similar to links, the short-cut version of an image link omits the link id and the alt text is interpreted as the link id (as seen in the previous example).
====

## Left-aligned images
~~~
\!\[kitty]\[\]
\].left

This text is displayed right next to the image.

It can span multiple paragraphs.

- list item 1
- list item 2

More regular text below the image.
\].none
~~~
].none
\
=>
\
![kitty][]
].left

This text is displayed right next to the image.

It can span multiple paragraphs.

- list item 1
- list item 2

More regular text below the image.
].none

[kitty]: res/image.png "kitty cat (title)"

TIP:
`.left` and `.none` are user-defined CSS classes.

It is possible to right-align images in a similar fashion (just edit the CSS).
===




# Tables

- All table lines must start with the `|` pipe character.
- A `|` after the last column is optional.
- The default column alignment is `center`.


## Simple table
   ~~~
   | Col A     | Col B (a wide column) | Col C
   | Data A1   | Data B1               | Data C1
   | Data A2   | Data B2               | Data C2
   | Data A3   | Data B3               | Data C3
   ] Simple table
   ~~~
=>
   | Col A     | Col B (a wide column) | Col C
   | Data A1   | Data B1               | Data C1
   | Data A2   | Data B2               | Data C2
   | Data A3   | Data B3               | Data C3
   ] Simple table


## Simple table with cell alignment
   ~~~
   |:This is Column A  | This is Column B | This is Column C:
   | Data A1           | Data B1          | Data C1
   | Data A2           | Data B2          | Data C2
   | Data A3           | Data B3          | Data C3
   ] Simple table with cell alignment
   ~~~
=>
   |:This is Column A  | This is Column B | This is Column C:
   | Data A1           | Data B1          | Data C1
   | Data A2           | Data B2          | Data C2
   | Data A3           | Data B3          | Data C3
   ] Simple table with cell alignment

+++
## Cell alignment in \-\-\- style table
   ~~~
   |-------------------|------------------|------------------|
   | This is Column A  | This is Column B | This is Column C |
   |:------------------|------------------|-----------------:|
   | Data A1           | Data B1          | Data C1          |
   | Data A2           | Data B2          | Data C2          |
   | Data A3           | Data B3          | Data C3          |
   |---------------------------------------------------------|
   ] Table cell alignment in \-\-\- style table
   ~~~
=>
   |-------------------|------------------|------------------|
   | This is Column A  | This is Column B | This is Column C |
   |:------------------|------------------|-----------------:|
   | Data A1           | Data B1          | Data C1          |
   | Data A2           | Data B2          | Data C2          |
   | Data A3           | Data B3          | Data C3          |
   |---------------------------------------------------------|
   ] Table cell alignment in \-\-\- style table


## Tables with different text styles, links, and images
   ~~~
   |-------------------|------------------|------------------|
   | This is Column A  | This is Column B | This is Column C |
   |:------------------|:----------------:|-----------------:|
   | `fixed`           | *emphasis1*      | _emphasis2_      |
   | ``fixed2``        | **strong1**      | __strong2__      |
   | ~subscript~       | ^superscript^    | ~~strikethrough~~|
   | [a link][1]       | ![kitty][]       | regular text     |
   |---------------------------------------------------------|
   ] Table cell text styles, links, and images
   ~~~
=>
   |-------------------|------------------|------------------|
   | This is Column A  | This is Column B | This is Column C |
   |:------------------|:----------------:|-----------------:|
   | `fixed`           | *emphasis1*      | _emphasis2_      |
   | ``fixed2``        | **strong1**      | __strong2__      |
   | ~subscript~       | ^superscript^    | ~~strikethrough~~|
   | [a link][1]       | ![kitty][]       | regular text     |
   |---------------------------------------------------------|
   ] Table cell text styles, links, and images

[kitty]: res/image.png "kitty cat (title)" w=64

+++
## Multi-line tables
   ~~~
   |------------------------------|---------------------|------------------|
   | This is Column A             | This is Column B    | This is Column C |
   |:-----------------------------|---------------------|-----------------:|
   | Data A11                     | Data A12            | Data A13         |
   | Data A21                     | Data A22            | Data A23         |
   | Data A31                     | Data A32            | Data A33         |
   |                              |+Data A32.line2      |+Data A33.line2   |
   |                              |+Data A32.line3      |+Data A33.line3   |
   |                              |                     |+Data A33.line4   |
   | Data A41                     | Data A42            | Data A43         |
   |+$\(inc!res/html_list.html)    |                     |                  |
   |-----------------------------------------------------------------------|
   ] Multi-line table cells via \+ modifier
   ~~~
=>
   |------------------------------|---------------------|------------------|
   | This is Column A             | This is Column B    | This is Column C |
   |:-----------------------------|---------------------|-----------------:|
   | Data A11                     | Data A12            | Data A13         |
   | Data A21                     | Data A22            | Data A23         |
   | Data A31                     | Data A32            | Data A33         |
   |                              |+Data A32.line2      |+Data A33.line2   |
   |                              |+Data A32.line3      |+Data A33.line3   |
   |                              |                     |+Data A33.line4   |
   | Data A41                     | Data A42            | Data A43         |
   |+$(inc!res/html_list.html)    |                     |                  |
   |-----------------------------------------------------------------------|
   ] Multi-line table cells via \+ modifier


+++
## Column widths
   ~~~
   |cols=15,10,25
   |-------------------|------------------|------------------|
   | This is Column A  | This is Column B | This is Column C |
   |:------------------|------------------|-----------------:|
   | Data A1           | Data B1          | Data C1          |
   | Data A2           | Data B2          | Data C2          |
   | Data A3           | Data B3          | Data C3          |
   |---------------------------------------------------------|
   ] Table cell widths via cols attribute
   ~~~
=>
   |cols=15,10,25
   |-------------------|------------------|------------------|
   | This is Column A  | This is Column B | This is Column C |
   |:------------------|------------------|-----------------:|
   | Data A1           | Data B1          | Data C1          |
   | Data A2           | Data B2          | Data C2          |
   | Data A3           | Data B3          | Data C3          |
   |---------------------------------------------------------|
   ] Table cell widths via cols attribute

## Headerless tables
~~~~
|noheader
| left | right
].noheader_table_example
~~~~
=>
|noheader
| left | right
].noheader_table_example

TIP:
Each table column can be styled differently by using the CSS `:nth-child(\<n\>)` feature, e.g.
~~~
.noheader_table_example .td:nth-child(1) {
    font-size: 20;
    font-weight: normal;
    background-color: #eee;
    padding-left: 0.25em;
    padding-right: 0.25em;
    border: 0px;
}
~~~
===


+++
## Complex tables (1)
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
(invisible comment) complex table example follows
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

>>>mytext
This is some regular text.

With multiple paragraphs.

~~~
a code block.
with multiple lines.
~~~
<<<

>>>mylist
1) list item 1a
2) list item 2a
    - sub list item 2a.1
3) list item 3a

>>>mylist2
1) list item 1b
2) list item 2b
    - sub list item 2b.1
3) list item 3b
4) list item 4b
<<<

>>>mytable
| First | Second | Third
| some  | table  | content
| another | row | of content
|         |     |+![kitty][]
<<<

   |-------------------|------------------|------------------|
   | This is Column A  | This is Column B | This is Column C |
   |:------------------|:-----------------|:-----------------|
   | $(buf!mytext)     | Data B1          | $(buf!mylist2)   |
   | Data A2           | $(buf!mylist)    | Data C2          |
   | Data A3           | Data B3          | $(buf!mytable)   |
   |---------------------------------------------------------|
   ] Complex tables via buffer redirection


+++
## Complex tables (2)
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
(invisible comment) complex table example 2 follows
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

>>>myminitable
| one | two
| 1.1 | 1.2
| 2.1 | 2.2

>>>mylist2
1) list item 1b
2) list item 2b
    - $(buf!myminitable)
    - list item 2b.2
3) list item 3b
<<<

>>>mynestedtable
   |-------------------|------------------|
   | Nested Column A   | Nested Column B  |
   |:------------------|:-----------------|
   | $(buf!mytext)     | $(buf!mylist2)   |
   | Data A2           | Data B2          |
   | Data A3           | $(buf!mytable)   |
   |--------------------------------------|
   ] Nested buffer table
<<<

   |----------------------|----------------------|
   | This is Column A     | This is Column B     |
   |:---------------------|:---------------------|
   | $(buf!mynestedtable) | $(buf!mynestedtable) |
   | $(buf!mynestedtable) | $(buf!mynestedtable) |
   |---------------------------------------------|
   ] Complex tables via buffer redirection 2



# CSS classes

The following CSS classes can be used to modify the document's appearance:


## Basic text styles and paragraphs
   |cols=20,20
   | Description                         | CSS Class
   |------------------------------------:|:-----------------
   |                           Main body | `.body`
   |                           Paragraph | `.p`
   |  Forced end of paragraph div (`\\`) | `.lf`
   |                              Header | `.h[1..9]`
   |             Header enumeration span | `.h[1..9]_enum`
   |                     Emphasized text | `.em`
   |               Emphasized text via _ | `.em2`
   |                         Strong text | `.strong`
   |                  Strong text via __ | `.strong2`
   |                  Strikethrough text | `.s`
   |                          Fixed text | `.code`
   |              Fixed text (double ``) | `.code2`
   |                      Subscript text | `.sub`
   |                    Superscript text | `.sup`
   |                   Preformatted text | `.pre`
   ].csstable Basic text styles and paragraphs


## Blockquotes and citations
   |cols=20,20
   | Description                         | CSS Class
   |------------------------------------:|:-----------------
   |                          Blockquote | `.blockquote`
   |          Blockquote with a citation | `.blockquote_cite`
   |                   Start of citation | `.blockquote_cite_begin`
   |                     End of citation | `.blockquote_cite_begin`
   |                     Citation footer | `.footer_cite`
   ].csstable Blockquotes and citations


## Images
   |cols=20,20
   | Description                         | CSS Class
   |------------------------------------:|:-----------------
   |                               Image | `.img`
   |              Link to original image | `.oimg`
   ].csstable Images


## Lists
   |cols=20,20
   | Description                         | CSS Class
   |------------------------------------:|:-----------------
   |                 Unordered root list | `.ulroot`
   |                  Unordered sub list | `.ulsub`
   |                   Ordered root list | `.olroot`
   |                    Ordered sub list | `.olsub`
   |                      Root list item | `.liroot`
   | Root list item followed by sub-list | `.lirootwithsub`
   |                 Last root list item | `.lirootlast`
   |                       Sub list item | `.lisub`
   |  Sub list item followed by sub-list | `.lisubwithsub`
   |                  Last sub list item | `.lisublast`
   ].csstable Lists


## Horizontal rules and printed page breaks
   |cols=20,20
   | Description                         | CSS Class
   |------------------------------------:|:-----------------
   |             Horizontal rule (`---`) | `.hr`
   |             Horizontal rule (`***`) | `.hr_asterisk`
   |           Horizontal rule (`* * *`) | `.hr_spaced_asterisk`
   |           Horizontal rule (`* * *`) | `.hr_spaced_asterisk_inner`
   |                  Printed page break | `.hr_pagebreak`
   ].csstable Horizontal rules and printed page breaks


## Subtitled paragraphs
   |cols=20,20
   | Description                         | CSS Class
   |------------------------------------:|:-----------------
   |              Sub-titled div (outer) | `.subdiv_outer`
   |              Sub-titled div (inner) | `.subdiv_inner`
   |              Sub-titled div (title) | `.subdiv_title`
   ].csstable Subtitled paragraphs


## Tables
   |cols=20,20
   | Description                         | CSS Class
   |------------------------------------:|:-----------------
   |                               Table | `.table`
   |                    Table header row | `.thtr`
   |                   Table header cell | `.thtd`  
   |                      Table data row | `.tr`
   |                     Table data cell | `.td`
   ].csstable Tables


## Footnotes
   |cols=20,20
   | Description                         | CSS Class
   |------------------------------------:|:-----------------
   |         Footnote link (superscript) | `.footnote_nr`
   |                Local Footnote table | `.footnote_table_local`
   |               Global Footnote table | `.footnote_table_global`
   |                  Footnote table row | `.footnote_tr`
   |        Footnote table cell (number) | `.footnote_td_nr`
   |          Footnote table cell (link) | `.footnote_td_link`
   |                 Footnote table link | `.footnote_a`
   ].csstable Footnotes


## Table of contents
   |cols=20,20
   | Description                         | CSS Class
   |------------------------------------:|:-----------------
   |               Table Of Contents div | `.toc_div`
   |   Table Of Contents enumeration div | `.toc_enum`
   ].csstable Table of contents

TIP:
The CSS class names used in the table of contents are prefixed by `toc_`, e.g. `toc_lirootwithsub`.
===

+++
## Admonitions
   |cols=20,20
   | Description                         | CSS Class
   |------------------------------------:|:-----------------
   |        Admonition `note:` outer div | `.note_s_div_outer`
   |        Admonition `NOTE:` outer div | `.note_l_div_outer`
   |   Admonition `important:` outer div | `.important_s_div_outer`
   |   Admonition `IMPORTANT:` outer div | `.important_l_div_outer`
   |         Admonition `tip:` outer div | `.tip_s_div_outer`
   |         Admonition `TIP:` outer div | `.tip_l_div_outer`
   |     Admonition `caution:` outer div | `.caution_s_div_outer`
   |     Admonition `CAUTION:` outer div | `.caution_l_div_outer`
   |     Admonition `warning:` outer div | `.warning_s_div_outer`
   |     Admonition `WARNING:` outer div | `.warning_l_div_outer`
   |            Admonition `note:` table | `.notediv_s_table`
   |            Admonition `NOTE:` table | `.notediv_l_table`
   |       Admonition `important:` table | `.important_s_div_table`
   |       Admonition `IMPORTANT:` table | `.important_l_div_table`
   |             Admonition `tip:` table | `.tip_s_div_table`
   |             Admonition `TIP:` table | `.tip_l_div_table`
   |         Admonition `caution:` table | `.caution_s_div_table`
   |         Admonition `CAUTION:` table | `.caution_l_div_table`
   |         Admonition `warning:` table | `.warning_s_div_table`
   |         Admonition `WARNING:` table | `.warning_l_div_table`
   |        Admonition `note:` table row | `.note_s_div_tr`
   |        Admonition `NOTE:` table row | `.note_l_div_tr`
   |   Admonition `important:` table row | `.important_s_div_tr`
   |   Admonition `IMPORTANT:` table row | `.important_l_div_tr`
   |         Admonition `tip:` table row | `.tip_s_div_tr`
   |         Admonition `TIP:` table row | `.tip_l_div_tr`
   |     Admonition `caution:` table row | `.caution_s_div_tr`
   |     Admonition `CAUTION:` table row | `.caution_l_div_tr`
   |     Admonition `warning:` table row | `.warning_s_div_tr`
   |     Admonition `WARNING:` table row | `.warning_l_div_tr`
   ].csstable Admonitions

   |cols=20,20
   | Description                         | CSS Class
   |------------------------------------:|:-----------------
   |            Admonition image (\<img\>) | `.infoimg`
   |      Admonition `note:` icon column | `.note_s_div_td_icon`
   |      Admonition `NOTE:` icon column | `.note_l_div_td_icon`
   | Admonition `important:` icon column | `.important_s_div_td_icon`
   | Admonition `IMPORTANT:` icon column | `.important_l_div_td_icon`
   |       Admonition `tip:` icon column | `.tip_s_div_td_icon`
   |       Admonition `TIP:` icon column | `.tip_l_div_td_icon`
   |   Admonition `caution:` icon column | `.caution_s_div_td_icon`
   |   Admonition `CAUTION:` icon column | `.caution_l_div_td_icon`
   |   Admonition `warning:` icon column | `.warning_s_div_td_icon`
   |   Admonition `WARNING:` icon column | `.warning_l_div_td_icon`
   |      Admonition `note:` text column | `.note_s_div_td_text`
   |      Admonition `NOTE:` text column | `.note_l_div_td_text`
   | Admonition `important:` text column | `.important_s_div_td_text`
   | Admonition `IMPORTANT:` text column | `.important_l_div_td_text`
   |       Admonition `tip:` text column | `.tip_s_div_td_text`
   |       Admonition `TIP:` text column | `.tip_l_div_td_text`
   |   Admonition `caution:` text column | `.caution_s_div_td_text`
   |   Admonition `CAUTION:` text column | `.caution_l_div_td_text`
   |   Admonition `warning:` text column | `.warning_s_div_td_text`
   |   Admonition `WARNING:` text column | `.warning_l_div_td_text`
   |        Admonition `note:` inner div | `.note_s_div_inner`
   |        Admonition `NOTE:` inner div | `.note_l_div_inner`
   |   Admonition `important:` inner div | `.important_s_div_inner`
   |   Admonition `IMPORTANT:` inner div | `.important_l_div_inner`
   |         Admonition `tip:` inner div | `.tip_s_div_inner`
   |         Admonition `TIP:` inner div | `.tip_l_div_inner`
   |     Admonition `caution:` inner div | `.caution_s_div_inner`
   |     Admonition `CAUTION:` inner div | `.caution_l_div_inner`
   |     Admonition `warning:` inner div | `.warning_s_div_inner`
   |     Admonition `WARNING:` inner div | `.warning_l_div_inner`
   ].csstable Admonitions (cont.)

+++
## Class name prefixes
The CSS class names can be prefixed via the `-cp \<class_prefix\>` command line option.


## Custom CSS classes

Each paragraph can be assigned a custom CSS class:
~~~
Some text

This is a div with a custom CSS class
].myclass

Some more text
~~~
=>
Some text

This is a div with a custom CSS class
].myclass

Some more text

TIP:
Custom CSS classes can also be used for advanced layouts, e.g. text floating right next to an image or a table.
===


# Creating a print version (PDF) of the document

While `md` itself cannot output PDF files, there are 3rd party tools that can transform the HTML output to a printable version of the document.

The `@media print` CSS selector can be used to select the paper size and margins, or fine-tune specific elements (e.g. automatically add page-breaks before or after certain elements, or prevent page breaks within certain elements).

The `+++` char sequence inserts a forced page break, while `+++col` / `+++/col` enable / disable multi-column mode.

NOTE:
If all that is needed is a redistributable, self-contained version of the document, the `-il` command line option can be used to embed any additional resources (images, CSS) into the HTML file. 
====

## Pandoc / LaTeX / wkhtmltopdf

These are free, open-source HTML-to-PDF solutions which seem to work fine with simpler documents but currently (October 2018) have issues with tables (LaTeX error message).

## Microsoft Print to PDF (Windows 10)

Windows 10 comes with a PDF printer driver that creates an excellent rendition of the document (including multi-column paragraphs).

It does have some (minor) drawbacks, though:
 - Document internal links will become plain text (which, to be honest, doesn't really matter when the document is actually printed)
 - The table of contents will not contain leading dots and page numbers (some of the print-related CSS is ignored)

## CutePDF (Windows 98/ME/2000/XP/2003/Vista/7/2012/8/8.1/10)

For earlier versions of Windows there are 3rd party PDF printer drivers like e.g. [^CutePDF][] (the output looks basically the same as with the Print to PDF driver).

[^CutePDF]: http://www.cutepdf.com/Products/CutePDF/writer.asp "Convert to PDF documents on the fly (FREE software for commercial and non-commercial use)"

## Prince

This is a 3rd party, standalone software[-][^prince] that is free for non-commercial use (available for Windows, MacOS, Linux, and FreeBSD).

It implements the W3 [GCPM][^cssgcpm3] spec, preserves external and document internal links, supports multi-column paragraphs, and can create a nice looking table of contents with `.` leaders and page numbers.

[^cssgcpm3]: https://www.w3.org/TR/css-gcpm-3/ "CSS Generated Content for Paged Media Module"
[^prince]: https://www.princexml.com/ "Convert your HTML documents to PDF"



$(buf!footnote)
$(buf!w3validator)

Document created on $(var:localdatetime)
].create
