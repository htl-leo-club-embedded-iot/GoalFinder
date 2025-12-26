/**
 * MarkdownConverter - Converts markdown text to HTML with support for tables,
 * code blocks, syntax highlighting, and custom signature blocks (alerts).
 * 
 * Features:
 * - Converts markdown syntax (headings, lists, bold, italic, etc.)
 * - Handles code blocks with syntax highlighting
 * - Supports custom signature blocks (positive, warning, negative, info)
 * - Generates tables with proper styling
 * - Navigation links between pages
 * - Source references with expandable list (#src[text](link))
 * 
 * Required global functions (must be defined in the consuming application):
 * - copyCodeBlock(button) - Handles copying code block content to clipboard
 * - toggleSources(id) - Toggles the expanded state of a sources block
 *   Example implementation:
 *   function toggleSources(id) {
 *       const element = document.getElementById(id);
 *       if (element) element.classList.toggle('expanded');
 *   }
 */
class MarkdownConverter {
	/** @type {path} - The path where the image assets are found */
	imagePath = `../../assets/img/`;

	/** @type {number} - Counter for unique heading IDs */
	headingCount;
	
	/** @type {string[]} - Flat array of all page paths for navigation */
	flatStructure;
	
	/** @type {Object[]} - Hierarchical structure of pages for sidebar navigation */
	contentStructure;
	
	/** @type {number} - Index of currently displayed page */
	currentPageIndex;
	
	/** @type {string[]} - Stores extracted code block content for copy functionality */
	codeBlocks;

	/**
	 * Creates a new MarkdownConverter instance
	 * @param {string[]} flatStructure - Flat array of page paths
	 * @param {Object[]} contentStructure - Hierarchical page structure (optional)
	 */
	constructor(flatStructure, contentStructure = []) {
		this.headingCount = 0;
		this.currentPageIndex = 0;
		this.flatStructure = flatStructure;
		this.contentStructure = contentStructure;
		this.codeBlocks = [];
	}

	/**
	 * Loads markdown content from a file path
	 * @async
	 * @param {string} path - URL path to the markdown file
	 * @returns {Promise<string>} - Raw markdown text content
	 */
	async loadMarkdown(path) {
		const response = await fetch(path);
		if (!response.ok) {
			console.error(`Failed to load markdown file: ${response.statusText}`);
			return "";
		}
		return await response.text();
	}

	/**
	 * Converts markdown text to HTML
	 * @param {string} md - Raw markdown text
	 * @param {string} path - Current page path (for navigation context)
	 * @returns {string} - Converted HTML string
	 */
	convert(md, path) {
		// Track current page position for navigation links
		this.currentPageIndex = this.flatStructure.indexOf(path);

		// Initialize state for conversion process
		this.headingCount = 0;
		this.codeBlocks = [];
		const lines = md.split("\n");
		const htmlLines = [];
		
		// Track state for different block types (lists, code blocks, tables, etc.)
		let inUnorderedList = false;
		let inOrderedList = false;
		let inCodeBlock = false;
		let inTable = false;
		let inCalculation = false;
		let calculationContent = [];
		let tableHeaders = [];
		let inFormattedCode = false;
		let currentFormattedCodeLang = "";
		let formattedCodeContent = [];
		let formattedCodePlainText = [];
		let pendingSources = [];

		// Process each line of markdown
		for (const line of lines) {
			// Formatted code block start (#codefs(lang))
			if (/#codefs\(([^)]+)\)/.test(line)) {
				inFormattedCode = true;
				formattedCodeContent = [];
				formattedCodePlainText = [];
				const match = line.match(/#codefs\(([^)]+)\)/);
				currentFormattedCodeLang = match[1];
			} else if (/#codefe/.test(line)) {
				// Formatted code block end - generate HTML with line numbers and copy button
				let lineNumbers = [];
				for (let i = 0; i < formattedCodeContent.length; i++) {
					const newLineNumber = `<span class="markdown-formatted-code-line-number">${i + 1}</span>`;
					lineNumbers.push(newLineNumber);
				}

				// Store code block for copy-to-clipboard functionality
				const codeBlockIndex = this.codeBlocks.length;
				this.codeBlocks.push(formattedCodePlainText.join("\n"));

				htmlLines.push(`<div class="markdown-formatted-code" data-code-index="${codeBlockIndex}">
                                    <div class="markdown-formatted-code-header">
                                        <p class="code-lang">${currentFormattedCodeLang}</p>
                                        <button class="code-copy-button" onclick="copyCodeBlock(this)">
                                            <img class="icon" src="${this.imagePath}copy.svg" alt="copy-icon">
                                            <span class="code-copy-text">Copy</span>
                                        </button>
                                    </div>
                                    <div class="markdown-formatted-code-content">
                                        <div class="markdown-formatted-code-line-numbers">${lineNumbers.join("<br>")}</div>
                                        <div class="markdown-formatted-code-content-lines">${this.colorFormatCode(formattedCodeContent).join("")}</div>
                                    </div>
                                </div>`);
				inFormattedCode = false;
			} else if (inFormattedCode) {
				// Accumulate code lines without escaping yet (escaping happens during colorFormatCode)
				formattedCodeContent.push(line);
				formattedCodePlainText.push(line);
			} else if (/^#cal/.test(line)) {
				// Toggle calculation block on/off
				if (inCalculation) {
					htmlLines.push(`<div class="markdown-calculation">${calculationContent.join("<br>")}</div>`);
				}
				inCalculation = !inCalculation;
				calculationContent = [];
			} else if (inCalculation) {
				// Accumulate calculation content
				calculationContent.push(this.escapeHtml(line));		} else if (/^#src\[([^\]]+)\]\(([^)]+)\)/.test(line)) {
			// Source reference - collect consecutive sources
			const match = line.match(/^#src\[([^\]]+)\]\(([^)]+)\)/);
			pendingSources.push({ text: match[1], link: match[2] });
		} else if (pendingSources.length > 0) {
			// Non-source line encountered - flush pending sources
			htmlLines.push(this.generateSourcesHTML(pendingSources));
			pendingSources = [];
			// Continue processing current line
			if (this.isTableRow(line)) {
				// Handle table parsing (check for headers and separators)
				if (!inTable) {
					// Close any open lists
					if (inUnorderedList) {
						htmlLines.push("</ul>");
						inUnorderedList = false;
					}
					if (inOrderedList) {
						htmlLines.push("</ol>");
						inOrderedList = false;
					}

					// Check if next line is separator to determine if this is a header
					const nextLineIndex = lines.indexOf(line) + 1;
					const nextLine = nextLineIndex < lines.length ? lines[nextLineIndex] : "";

					if (this.isTableSeparator(nextLine)) {
						htmlLines.push('<table class="markdown-table">');
						htmlLines.push("<thead>");
						htmlLines.push(this.parseTableRow(line, true));
						htmlLines.push("</thead>");
						htmlLines.push("<tbody>");
						inTable = true;
						tableHeaders = this.extractTableHeaders(line);
					} else {
						htmlLines.push('<table class="markdown-table">');
						htmlLines.push("<tbody>");
						htmlLines.push(this.parseTableRow(line, false));
						inTable = true;
					}
				} else {
					if (!this.isTableSeparator(line)) {
						htmlLines.push(this.parseTableRow(line, false));
					}
				}
			} else if (/^(\*|\-|\+)\s/.test(line)) {
				if (!inUnorderedList) {
					htmlLines.push('<ul class="markdown-list">');
					inUnorderedList = true;
				}
				if (inOrderedList) {
					htmlLines.push("</ol>");
					inOrderedList = false;
				}
				htmlLines.push(this.parseLine(line));
			} else if (/^\d+\.\s/.test(line)) {
				if (!inOrderedList) {
					htmlLines.push('<ol class="markdown-ordered-list-spaced">');
					inOrderedList = true;
				}
				if (inUnorderedList) {
					htmlLines.push("</ul>");
					inUnorderedList = false;
				}
				htmlLines.push(this.parseLine(line));
			} else {
				if (inTable) {
					htmlLines.push("</tbody>");
					htmlLines.push("</table>");
					inTable = false;
					tableHeaders = [];
				}
				if (inUnorderedList) {
					htmlLines.push("</ul>");
					inUnorderedList = false;
				}
				if (inOrderedList) {
					htmlLines.push("</ol>");
					inOrderedList = false;
				}
				htmlLines.push(this.parseLine(line));
			}			} else if (/^```/.test(line)) {
				// Toggle standard code block on/off
				if (inCodeBlock) {
					htmlLines.push("</pre>");
				} else {
					htmlLines.push('<pre class="markdown-code-block">');
				}
				inCodeBlock = !inCodeBlock;
			} else if (inCodeBlock) {
				// Add escaped code line to block
				htmlLines.push(this.escapeHtml(line));
			} else if (this.isTableRow(line)) {
				// Handle table parsing (check for headers and separators)
				if (!inTable) {
					// Close any open lists
					if (inUnorderedList) {
						htmlLines.push("</ul>");
						inUnorderedList = false;
					}
					if (inOrderedList) {
						htmlLines.push("</ol>");
						inOrderedList = false;
					}

					// Check if next line is separator to determine if this is a header
					const nextLineIndex = lines.indexOf(line) + 1;
					const nextLine = nextLineIndex < lines.length ? lines[nextLineIndex] : "";

					if (this.isTableSeparator(nextLine)) {
						// This is a header row - create table with thead
						htmlLines.push('<table class="markdown-table">');
						htmlLines.push("<thead>");
						htmlLines.push(this.parseTableRow(line, true));
						htmlLines.push("</thead>");
						htmlLines.push("<tbody>");
						inTable = true;
						tableHeaders = this.extractTableHeaders(line);
					} else {
						// This is a regular table without headers
						htmlLines.push('<table class="markdown-table">');
						htmlLines.push("<tbody>");
						htmlLines.push(this.parseTableRow(line, false));
						inTable = true;
					}
				} else {
					// Continue adding rows to active table (skip separator lines)
					if (!this.isTableSeparator(line)) {
						htmlLines.push(this.parseTableRow(line, false));
					}
				}
			} else if (/^(\*|\-|\+)\s/.test(line)) {
				// Unordered list item
				if (!inUnorderedList) {
					htmlLines.push('<ul class="markdown-list">');
					inUnorderedList = true;
				}

				// Close ordered list if active
				if (inOrderedList) {
					htmlLines.push("</ol>");
					inOrderedList = false;
				}
				htmlLines.push(this.parseLine(line));
			} else if (/^\d+\.\s/.test(line)) {
				// Ordered list item
				if (!inOrderedList) {
					htmlLines.push('<ol class="markdown-ordered-list-spaced">');
					inOrderedList = true;
				}
				// Close unordered list if active
				if (inUnorderedList) {
					htmlLines.push("</ul>");
					inUnorderedList = false;
				}
				htmlLines.push(this.parseLine(line));
			} else {
				// Regular content line - close any open blocks
				if (inTable) {
					htmlLines.push("</tbody>");
					htmlLines.push("</table>");
					inTable = false;
					tableHeaders = [];
				}
				if (inUnorderedList) {
					htmlLines.push("</ul>");
					inUnorderedList = false;
				}
				if (inOrderedList) {
					htmlLines.push("</ol>");
					inOrderedList = false;
				}
				htmlLines.push(this.parseLine(line));
			}
		}

		if (inUnorderedList) {
			htmlLines.push("</ul>");
		}
		if (inOrderedList) {
			htmlLines.push("</ol>");
		}
		if (inCodeBlock) {
			htmlLines.push("</pre>");
		}
		if (inTable) {
			htmlLines.push("</tbody>");
			htmlLines.push("</table>");
		}
		if (inCalculation) {
			// Close any unclosed calculation block
			htmlLines.push(`<div class="markdown-calculation">${calculationContent.join("<br>")}</div>`);
		}
		if (pendingSources.length > 0) {
			// Flush any remaining sources
			htmlLines.push(this.generateSourcesHTML(pendingSources));
		}

		// Process signature block markers and return final HTML
		return this.processSignatureBlocks(htmlLines.join("\n"));
	}

	/**
	 * Applies syntax highlighting to code lines
	 * @param {string[]} codeLines - Array of code lines to highlight
	 * @returns {string[]} - Array of HTML lines with syntax highlighting applied
	 */
	colorFormatCode(codeLines = []) {
		let returnLines = [];

		const escapeHtml = (str) => {
			return str.replace(/&/g, "&amp;").replace(/</g, "&lt;").replace(/>/g, "&gt;").replace(/"/g, "&quot;").replace(/'/g, "&#039;");
		};

		// List of programming language keywords for syntax highlighting
		const keywords = [
			"auto",
			"break",
			"case",
			"char",
			"const",
			"continue",
			"default",
			"do",
			"double",
			"else",
			"enum",
			"extern",
			"float",
			"for",
			"goto",
			"if",
			"int",
			"long",
			"register",
			"return",
			"short",
			"signed",
			"sizeof",
			"static",
			"struct",
			"switch",
			"typedef",
			"union",
			"unsigned",
			"void",
			"volatile",
			"while",
			"asm",
			"bool",
			"catch",
			"class",
			"const_cast",
			"delete",
			"dynamic_cast",
			"explicit",
			"export",
			"false",
			"friend",
			"inline",
			"mutable",
			"namespace",
			"new",
			"operator",
			"private",
			"protected",
			"public",
			"reinterpret_cast",
			"static_cast",
			"string",
			"template",
			"this",
			"throw",
			"true",
			"try",
			"typeid",
			"typename",
			"using",
			"virtual",
			"wchar_t",
		];

		// Regex to tokenize code - identifies comments, strings, numbers, identifiers, operators, etc.
		const tokenRegex = /\/\/.*?$|\/\*.*?\*\/|"[^"]*"|'[^']*'|#\w+|\b\d+(?:\.\d+)?\b|\b\w+(?=\.)|(?<=\.)\w+\b|[{}()[\];,.]|[-+\/*=%.&|!<>?:]+|\b\w+\b|\s+/g;

		// Process each code line for syntax highlighting
		codeLines.forEach((line) => {
			let newline = [];
			let lastIndex = 0;
			let previousToken = "";
			tokenRegex.lastIndex = 0;

			if (line == "") {
				newline.push("<br>");
			} else {
				// Tokenize the line and apply appropriate styles
				while (true) {
					const match = tokenRegex.exec(line);
					if (!match) break;

					// Add any unmatched text before this token
					if (match.index > lastIndex) {
						const unmatched = line.substring(lastIndex, match.index);
						newline.push(escapeHtml(unmatched));
					}

					const token = match[0];
					let colored = false;

					// Apply color based on token type
					if (token.startsWith("//") || token.startsWith("/*")) {
						newline.push(`<span class="markdown-code-comment">${escapeHtml(token)}</span>`);
						colored = true;
					} else if (token.startsWith("#")) {
						newline.push(`<span class="markdown-code-hash">${escapeHtml(token)}</span>`);
						colored = true;
					} else if (token.startsWith('"') || token.startsWith("'")) {
						newline.push(`<span class="markdown-code-qmark">${escapeHtml(token)}</span>`);
						colored = true;
					} else if (/^\d+(?:\.\d+)?$/.test(token)) {
						// Number literal
						newline.push(`<span class="markdown-code-number">${escapeHtml(token)}</span>`);
						colored = true;
					} else if (/^\w+$/.test(token) && previousToken === ".") {
						// Property/method name (comes after a dot)
						newline.push(`<span class="markdown-code-property">${escapeHtml(token)}</span>`);
						colored = true;
					} else if (/^\w+$/.test(token) && line.charAt(match.index + token.length) === ".") {
						// Object/class name (followed by a dot)
						newline.push(`<span class="markdown-code-object">${escapeHtml(token)}</span>`);
						colored = true;
					} else if (keywords.includes(token)) {
						// Programming keyword
						newline.push(`<span class="markdown-code-keyword">${escapeHtml(token)}</span>`);
						colored = true;
					} else if (/^[\[\]]$/.test(token)) {
						// Square brackets
						newline.push(`<span class="markdown-code-square-brackets">${escapeHtml(token)}</span>`);
						colored = true;
					} else if (/^[()]$/.test(token)) {
						// Round brackets/parentheses
						newline.push(`<span class="markdown-code-brackets">${escapeHtml(token)}</span>`);
						colored = true;
					} else if (/^[{}]$/.test(token)) {
						// Curly brackets/braces
						newline.push(`<span class="markdown-code-curly-brackets">${escapeHtml(token)}</span>`);
						colored = true;
					} else if (token === ".") {
						// Dot operator
						newline.push(`<span class="markdown-code-dot">${escapeHtml(token)}</span>`);
						colored = true;
					} else if (/^<.*>$/.test(token)) {
						// HTML tag or template
						newline.push(token);
						colored = true;
					}

					// Handle uncolored tokens
					if (!colored) {
						if (/^&[a-zA-Z]+;/.test(token)) {
							// HTML entity
							newline.push(`<span>${token}</span>`);
						} else {
							// Regular text
							newline.push(`<span>${escapeHtml(token)}</span>`);
						}
					}

					previousToken = token.trim();
					lastIndex = match.index + token.length;
				}
			}

			// Add any remaining text after last token
			if (lastIndex < line.length) {
				const unmatched = line.substring(lastIndex);
				newline.push(escapeHtml(unmatched));
			}

			// Wrap line in paragraph with pre-formatted whitespace
			const lineHtml = newline.join("");
			returnLines.push(`<p style="white-space: pre;">${lineHtml}</p>`);
		});

		return returnLines;
	}

	/**
	 * Detects and converts a single line based on markdown syntax
	 * Handles: headings, lists, signatures, navigation, paragraphs
	 * @param {string} line - A single line of markdown text
	 * @returns {string} - Converted HTML or signature marker
	 */
	parseLine(line) {
		// Handle custom signature block markers
		// Positive signatures: #+ prefix
		if (/^#\+h\s/.test(line)) {
			const content = line.replace(/^#\+h\s/, "");
			return `SIGNATURE_START:positive:heading:${this.parseInline(content)}`;
		}
		if (/^#\+b\s/.test(line)) {
			const content = line.replace(/^#\+b\s/, "");
			return `SIGNATURE_START:positive:body:${this.parseInline(content)}`;
		}

		// Warning signatures: #w prefix
		if (/^#wh\s/.test(line)) {
			const content = line.replace(/^#wh\s/, "");
			return `SIGNATURE_START:warning:heading:${this.parseInline(content)}`;
		}
		if (/^#wb\s/.test(line)) {
			const content = line.replace(/^#wb\s/, "");
			return `SIGNATURE_START:warning:body:${this.parseInline(content)}`;
		}

		// Negative/Error signatures: #- prefix
		if (/^#-h\s/.test(line)) {
			const content = line.replace(/^#-h\s/, "");
			return `SIGNATURE_START:negative:heading:${this.parseInline(content)}`;
		}
		if (/^#-b\s/.test(line)) {
			const content = line.replace(/^#-b\s/, "");
			return `SIGNATURE_START:negative:body:${this.parseInline(content)}`;
		}

		// Info signatures: #i prefix
		if (/^#ih\s/.test(line)) {
			const content = line.replace(/^#ih\s/, "");
			return `SIGNATURE_START:info:heading:${this.parseInline(content)}`;
		}
		if (/^#ib\s/.test(line)) {
			const content = line.replace(/^#ib\s/, "");
			return `SIGNATURE_START:info:body:${this.parseInline(content)}`;
		}

		// Button signatures: #b prefix with optional ID
		if (/^#bh\(([^)]+)\)\s/.test(line)) {
			const match = line.match(/^#bh\(([^)]+)\)\s(.*)$/);
			if (!match) {
				return `<p class="markdown-paragraph">${this.parseInline(line)}</p>`;
			}
			const id = match[1];
			const content = match[2];
			return `SIGNATURE_START:button:heading:${this.parseInline(content)}:${id}`;
		}
		if (/^#bb\s/.test(line)) {
			const content = line.replace(/^#bb\s/, "");
			return `SIGNATURE_START:button:body:${this.parseInline(content)}`;
		}

		// Headings
		if (/^#{1,6}\s/.test(line)) {
			const level = line.match(/^#{1,6}/)[0].length;
			const content = line.replace(/^#{1,6}\s/, "");
			const headingId = this.headingCount++;
			return `<h${level} id="${headingId}" class="markdown-heading">${this.parseInline(content)}</h${level}>`;
		}

		// Unordered Lists
		if (/^(\*|\-|\+)\s/.test(line)) {
			const content = line.replace(/^(\*|\-|\+)\s/, "");
			return `<li class="markdown-list-item">${this.parseInline(content)}</li>`;
		}

		// Ordered Lists
		if (/^\d+\.\s/.test(line)) {
			const content = line.replace(/^\d+\.\s/, "");
			return `<li class="markdown-ordered-list-item">${this.parseInline(content)}</li>`;
		}

		// Line breaks
		if (line.trim() === "") {
			return "<br>";
		}

		// End signature - navigation links to previous/next pages
		if (line.trim() === "#end") {
			let item = `<div class="markdown-end-container">`;

			// If previous item exists
			if (this.currentPageIndex !== 0) {
				const previousPath = this.flatStructure[this.currentPageIndex - 1];
				const previousName = this.getPageName(previousPath);
				item += `<div class="markdown-end-next" data-path="${previousPath}">
                            <img class="markdown-end-icon-previous" src="${imagePath}arrow.svg" alt="next-icon"></img>
                            <div class="markdown-end-content-p">
                                <p class="markdown-end-label">Previous</p>
                                <p class="markdown-end-title">${previousName}</p>
                            </div>
                         </div>`;
			}

			// If next item exists
			if (this.currentPageIndex !== this.flatStructure.length - 1) {
				const nextPath = this.flatStructure[this.currentPageIndex + 1];
				const nextName = this.getPageName(nextPath);
				item += `<div class="markdown-end-next" data-path="${nextPath}">
                            <div class="markdown-end-content-n">
                                <p class="markdown-end-label">Next</p>
                                <p class="markdown-end-title">${nextName}</p>
                            </div>
                            <img class="markdown-end-icon" src="${this.imagePath}arrow.svg" alt="next-icon"></img>
                         </div>`;
			}

			item += `</div>`;
			return item;
		}

		return `<p class="markdown-paragraph">${this.parseInline(line)}</p>`;
	}

	/**
	 * Parses inline markdown elements within text
	 * @param {string} text - Text containing inline markdown
	 * @returns {string} - HTML with inline elements converted
	 */
	parseInline(text) {
		// Process inline markdown elements in order of precedence
		return text
			// Images with optional dimensions: ![alt](src) or ![alt](src)(300x200)
			.replace(/!\[([^\]]*)\]\(([^)]+)\)(\(([^)]+)\))?/g, (match, alt, src, _, dimensions) => {
				let imgTag = `<img src="${src}" alt="${alt}" class="markdown-image"`;

				if (dimensions) {
					// Parse dimensions like "300x200" or "300" (width only)
					const dimMatch = dimensions.match(/^(\d+)(?:x(\d+))?$/);
					if (dimMatch) {
						const width = dimMatch[1];
						const height = dimMatch[2];
						imgTag += ` width="${width}"`;
						if (height) {
							imgTag += ` height="${height}"`;
						}
					}
				}

				return imgTag + " />";
			}) // Images with optional dimensions
			.replace(/\[\[([^\]]+)\]\(([^)]+)\)\]/g, '<span id="$2" class="markdown-button">$1</span>') // Button links with ID
			.replace(/\[([^\]]+)\]\(([^)]+)\)/g, '<a href="$2" target="_blank" class="markdown-link">$1</a>') // Links
			.replace(/~~(.*?)~~/g, '<del class="markdown-strikethrough">$1</del>') // Strikethrough
			.replace(/\*\*(.*?)\*\*/g, '<strong class="markdown-bold">$1</strong>') // Bold
			.replace(/\*(.*?)\*/g, '<em class="markdown-italic">$1</em>') // Italic
			.replace(/`(.*?)`/g, '<code class="markdown-code">$1</code>'); // Inline code
	}

	/**
	 * Escapes HTML special characters to prevent injection
	 * @param {string} text - Text to escape
	 * @returns {string} - Escaped HTML-safe text
	 */
	escapeHtml(text) {
		return text.replace(/&/g, "&amp;").replace(/</g, "&lt;").replace(/>/g, "&gt;").replace(/"/g, "&quot;").replace(/'/g, "&#039;");
	}

	/**
	 * Processes and merges signature block markers into HTML
	 * Combines heading and body parts into complete signature elements
	 * @param {string} html - HTML string containing signature markers
	 * @returns {string} - HTML with signature blocks converted
	 */
	processSignatureBlocks(html) {
		const lines = html.split("\n");
		const result = [];
		let currentSignature = null;

		// Process each line, looking for signature markers
		for (const line of lines) {
			if (line.startsWith("SIGNATURE_START:")) {
				// Parse marker format: SIGNATURE_START:type:part:content[:id]
				const parts = line.split(":");
				const type = parts[1];
				const part = parts[2];
				const content = parts[3];
				const id = parts[4]; // For button signatures with ID

				if (currentSignature && currentSignature.type === type) {
					// Add to existing signature of same type
					if (part === "heading") {
						currentSignature.heading = content;
						if (id) currentSignature.id = id;
					} else if (part === "body") {
						currentSignature.bodies.push(content);
					}
				} else {
					// Finish previous signature if exists
					if (currentSignature) {
						result.push(this.generateSignatureHTML(currentSignature));
					}

					// Start new signature
					currentSignature = {
						type: type,
						heading: part === "heading" ? content : null,
						bodies: part === "body" ? [content] : [],
						id: id || null,
					};
				}
			} else {
				// Regular content line - finish current signature if exists
				if (currentSignature) {
					result.push(this.generateSignatureHTML(currentSignature));
					currentSignature = null;
				}
				result.push(line);
			}
		}

		// Finish final signature if exists
		if (currentSignature) {
			result.push(this.generateSignatureHTML(currentSignature));
		}

		return result.join("\n");
	}

	/**
	 * Generates complete HTML for a signature block
	 * @param {Object} signature - Signature object with type, heading, bodies, id
	 * @returns {string} - Complete HTML for the signature block
	 */
	generateSignatureHTML(signature) {
		if (signature.type === "button") {
			// Button signatures: Create container div with optional ID and heading/body sections
			let html = `<div class="markdown-button-container"${signature.id ? ` id="${signature.id}"` : ""}>`;

			// Add button heading if present
			if (signature.heading) {
				html += `<div class="markdown-button-heading">${signature.heading}</div>`;
			}

			// Add all body paragraphs
			for (const body of signature.bodies) {
				html += `<div class="markdown-button-body">${body}</div>`;
			}

			html += "</div>";
			return html;
		} else {
			// Alert signatures (positive, warning, negative, info): Create semantic alert box
			let html = `<div class="markdown-${signature.type}">`;

			// If heading exists, add header section with icon and title
			if (signature.heading) {
				html += `<div class="markdown-signature-header">`;
				// Choose icon image based on signature type (match CSS custom properties)
				if (signature.type === "positive") {
					html += `<img class="icon" src="${this.imagePath}circle-check.svg" alt="positive-icon"></img>`;
				} else if (signature.type === "warning") {
					html += `<img class="icon" src="${this.imagePath}triangle-exclamation.svg" alt="warning-icon"></img>`;
				} else if (signature.type === "negative") {
					html += `<img class="icon" src="${this.imagePath}cancel.svg" alt="negative-icon"></img>`;
				} else if (signature.type === "info") {
					html += `<img class="icon" src="${this.imagePath}circle-info.svg" alt="info-icon"></img>`;
				}
				html += `<h1 class="markdown-signature-heading">${signature.heading}</h1>`;
				html += `</div>`;
			}

			// Add all body paragraphs
			for (const body of signature.bodies) {
				html += `<p class="markdown-signature-body">${body}</p>`;
			}

			html += "</div>";
			return html;
		}
	}

	/**
	 * Retrieves the display name of a page from its path
	 * Searches the hierarchical content structure recursively
	 * @param {string} path - Path to the page
	 * @returns {string} - Display name of the page or "Unknown Page"
	 */
	getPageName(path) {
		const findName = (items) => {
			for (const item of items) {
				if (item.path === path) {
					return item.name;
				}
				if (item.children) {
					const found = findName(item.children);
					if (found) return found;
				}
			}
			return null;
		};
		return findName(this.contentStructure) || "Unknown Page";
	}

	/**
	 * Generates HTML for a sources block with expandable list
	 * @param {Object[]} sources - Array of source objects with text and link properties
	 * @returns {string} - HTML for the sources block
	 */
	generateSourcesHTML(sources) {
		const count = sources.length;
		const displayCount = count > 9 ? "9+" : count.toString();
		const uniqueId = `sources-${Date.now()}-${Math.random().toString(36).substr(2, 9)}`;

		let html = `<div class="markdown-sources" id="${uniqueId}">`;
		html += `<div class="markdown-sources-header" onclick="toggleSources('${uniqueId}')">`;
		html += `<span class="markdown-sources-count">${displayCount}</span>`;
		html += `<span class="markdown-sources-label">Sources</span>`;
		html += `</div>`;
		html += `<div class="markdown-sources-list">`;

		for (let i = 0; i < sources.length; i++) {
			const source = sources[i];
			html += `<a href="${source.link}" target="_blank" class="markdown-sources-item">`;
			html += `<span class="markdown-sources-item-number">${i + 1}.</span>`;
			html += `<span class="markdown-sources-item-text">${this.escapeHtml(source.text)}</span>`;
			html += `</a>`;
		}

		html += `</div>`;
		html += `</div>`;

		return html;
	}

	/**
	 * Checks if a line is a valid table row
	 * @param {string} line - Line to check
	 * @returns {boolean} - True if line contains pipe characters
	 */
	isTableRow(line) {
		// A table row contains pipe separators and is not empty
		return line.trim().includes("|") && line.trim().length > 0;
	}

	/**
	 * Checks if a line is a table separator (|---|---| format)
	 * @param {string} line - Line to check
	 * @returns {boolean} - True if line matches table separator pattern
	 */
	isTableSeparator(line) {
		// Separator must contain pipes, hyphens, colons (for alignment), and whitespace only
		return /^\s*\|[\s\-\|:]+\|\s*$/.test(line);
	}

	/**
	 * Extracts header names from a table header row
	 * @param {string} line - Table header row
	 * @returns {string[]} - Array of header cell values
	 */
	extractTableHeaders(line) {
		// Split by pipe, trim whitespace, remove empty cells (leading/trailing pipes)
		return line
			.split("|")
			.map((cell) => cell.trim())
			.filter((cell) => cell.length > 0);
	}

	/**
	 * Parses a table row into HTML table row markup
	 * @param {string} line - Table row line with pipe-separated cells
	 * @param {boolean} isHeader - Whether this is a header row (th) or data row (td)
	 * @returns {string} - HTML table row string
	 */
	parseTableRow(line, isHeader) {
		// Extract cells from pipe-separated row
		const cells = line
			.split("|")
			.map((cell) => cell.trim())
			.filter((cell) => cell.length > 0);
		
		// Use <th> for header rows, <td> for data rows
		const tag = isHeader ? "th" : "td";
		const className = isHeader ? "markdown-table-header" : "markdown-table-cell";

		// Build table row HTML with proper styling classes
		let html = '<tr class="markdown-table-row">';
		for (const cell of cells) {
			// Apply inline markdown parsing to cell content (bold, italic, links, etc.)
			html += `<${tag} class="${className}">${this.parseInline(cell)}</${tag}>`;
		}
		html += "</tr>";

		return html;
	}
}

export default MarkdownConverter;
