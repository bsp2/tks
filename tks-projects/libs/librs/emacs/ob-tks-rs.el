;;; ob-tks-rs.el --- Babel Functions for Remote TKS Scripts    -*- lexical-binding: t; -*-

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

;; Provides a way to remote-evaluate TKS scripts in Org mode.

;;; Usage:

;; Add to your Emacs config:

;; (org-babel-do-load-languages
;;  'org-babel-load-languages
;;  '((tks-rs . t)))

;; (todo) update comment
;; In addition to the normal header arguments, ob-tks-rs also provides
;; :cmd-line, :in-file and :args.
;; :cmd-line allows one to pass other flags to the tks command like "-ef" for adding additional files
;; :host sets the remote process host name (default="host")
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

(defconst org-babel-header-args:tks-rs
  '((:cmd-line . :any)
    (:host     . :any)
    (:in-file  . :any)
    (:args     . :any)
    )
  "Tks specific header arguments.")

(defvar org-babel-default-header-args:tks-rs '()
  "Default arguments for evaluating a tks remote source block.")

(defun org-babel-execute:tks-rs (body params)
  "Execute a block of TKS remote code with Org Babel.
BODY is the source inside a tks source block and PARAMS is an
association list over the source block configurations.  This
function is called by `org-babel-execute-src-block'."
  (message "Executing tks-rs source code block")
  (let* ((result-params (cdr (assq :result-params params)))
         (cmd-line (cdr (assq :cmd-line params)))
         (args (cdr (assq :args params)))
         (host (cdr (assq :host params)))
         (in-file (cdr (assq :in-file params)))
	 (code-file (let ((file (org-babel-temp-file "tks-rs-" ".tks")))
                      (with-temp-file file
			(insert body))
		      file))
	 (invoke-file (let ((file (org-babel-temp-file "tks-rs-invoke-" ".tks")))
                        (with-temp-file file
			  (insert (concat "RemoteScriptClient.EvalFileToStdout(\"" (if (not host) "host" host) "\", \"" code-file "\");")))
		        file))
	 (stdin (let ((stdin (cdr (assq :stdin params))))
		  (when stdin
		    (let ((tmp (org-babel-temp-file "tks-rs-stdin-"))
			  (res (org-babel-ref-resolve stdin)))
		      (with-temp-file tmp
			(insert res))
		      tmp))))
         (cmd (mapconcat #'identity
			 (remq nil
			       (list org-babel-tks-command
				     ;(append cmd-line '("-ep" "lib:rs"))
                                     (concat cmd-line "-ep lib:rs")
				     (format "\"%s\"" invoke-file)
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
	   (let ((tmp (org-babel-temp-file "tks-rs-results-")))
	     (with-temp-file tmp (insert results))
	     (org-babel-import-elisp-from-file tmp)))))
     (org-babel-pick-name
      (cdr (assq :colname-names params)) (cdr (assq :colnames params)))
     (org-babel-pick-name
      (cdr (assq :rowname-names params)) (cdr (assq :rownames params))))))

(provide 'ob-tks-rs)

;;; ob-tks-rs.el ends here
