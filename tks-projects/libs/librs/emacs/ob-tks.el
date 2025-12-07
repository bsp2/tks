;;; ob-tks.el --- Babel Functions for TKS Scripts    -*- lexical-binding: t; -*-

;; [16Aug2024] based on 'ob-sed.el'  <https://raw.githubusercontent.com/bzg/org-mode/main/lisp/ob-sed.el>

;; Copyright (C) 2015-2023 Free Software Foundation, Inc.

;; Author: Bjarte Johansen
;; Keywords: literate programming, reproducible research

;; This file is part of GNU Emacs.

;; GNU Emacs is free software: you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation, either version 3 of the License, or
;; (at your option) any later version.

;; GNU Emacs is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.

;; You should have received a copy of the GNU General Public License
;; along with GNU Emacs.  If not, see <https://www.gnu.org/licenses/>.

;;; Commentary:

;; Provides a way to evaluate TKS scripts in Org mode.

;;; Usage:

;; Add to your Emacs config:

;; (org-babel-do-load-languages
;;  'org-babel-load-languages
;;  '((tks . t)))

;; (todo) update comment
;; In addition to the normal header arguments, ob-tks also provides
;; :cmd-line, :in-file and :args.
;; :cmd-line allows one to pass other flags to the tks command like "-ef" or "-ep" for adding additional files
;; :in-file is a header arguments that allows one to tell Org Babel which file the tks script to act on.
;; :args passes arguments to the executed script (which will be available in the Arguments array)

;;; Code:

(require 'org-macs)
;(org-assert-version)

(require 'ob)

(defvar org-babel-tks-command "tks"
  "Name of the tks executable command.")

(defvar org-babel-tangle-lang-exts)
(add-to-list 'org-babel-tangle-lang-exts '("tks" . "tks"))

(defconst org-babel-header-args:tks
  '((:cmd-line . :any)
    (:in-file  . :any)
    (:args     . :any)
    )
  "Tks specific header arguments.")

(defvar org-babel-default-header-args:tks '()
  "Default arguments for evaluating a tks source block.")

(defun org-babel-execute:tks (body params)
  "Execute a block of TKS code with Org Babel.
BODY is the source inside a tks source block and PARAMS is an
association list over the source block configurations.  This
function is called by `org-babel-execute-src-block'."
  (message "Executing tks source code block")
  (let* ((result-params (cdr (assq :result-params params)))
         (cmd-line (cdr (assq :cmd-line params)))
         (args (cdr (assq :args params)))
         (in-file (cdr (assq :in-file params)))
	 (code-file (let ((file (org-babel-temp-file "tks-" ".tks")))
                      (with-temp-file file
			(insert body))
		      file))
	 (stdin (let ((stdin (cdr (assq :stdin params))))
		  (when stdin
		    (let ((tmp (org-babel-temp-file "tks-stdin-"))
			  (res (org-babel-ref-resolve stdin)))
		      (with-temp-file tmp
			(insert res))
		      tmp))))
         (cmd (mapconcat #'identity
			 (remq nil
			       (list org-babel-tks-command
				     cmd-line
				     (format "\"%s\"" code-file)
				     args
				     in-file))
			 " ")))
    (org-babel-reassemble-table
     (let ((results
            (cond
             (stdin (with-temp-buffer
                      (call-process-shell-command cmd stdin (current-buffer))
                      (buffer-string)))
             (t (org-babel-eval cmd "")))))
       (when results
         (org-babel-result-cond result-params
	   results
	   (let ((tmp (org-babel-temp-file "tks-results-")))
	     (with-temp-file tmp (insert results))
	     (org-babel-import-elisp-from-file tmp)))))
     (org-babel-pick-name
      (cdr (assq :colname-names params)) (cdr (assq :colnames params)))
     (org-babel-pick-name
      (cdr (assq :rowname-names params)) (cdr (assq :rownames params))))))

(provide 'ob-tks)

;;; ob-tks.el ends here
