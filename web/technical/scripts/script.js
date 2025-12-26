// Comment to commit on new branch

// Import necessary modules for content browsing and markdown conversion
import MarkdownConverter from "../../tools/markdown-converter/markdown-converter.js";
import ContentBrowser from "../../tools/content-browser/content-browser.js";

// Global variable to store current markdown content
let currentMarkdownContent = ""; // for copy button

// Global content browser instance
let browser = new ContentBrowser();

// Global markdown converter instance
let converter;
const preview = document.getElementById("markdown-container");

let browserContainer;

// Track currently selected elements for O(1) deselection
let currentlySelectedTopic = null;
let currentlySelectedCategory = null;

// Assign mode switch variable to the button for improved onclick event
window.lightDarkModeToggle = lightDarkModeToggle;

// Make function available globally for onclick handler
window.copyButtonTrigger = copyButtonTrigger;

// Make copy code block function available globally for onclick handler
window.copyCodeBlock = copyCodeBlock;

// Make searchbar functions available globally for onfocus/onblur/oninput handlers
window.searchbarFocus = searchbarFocus;
window.searchbarBlur = searchbarBlur;
window.searchbarSearch = searchbarSearch;

/**
 * Gets all the markdown headings to display in the right panel
 * @param {Path} path
 * @returns
 */
async function getMarkdownHeaders(path) {
	let headings = [];
	let id = 0;
	let markdown = await converter.loadMarkdown(path);

	for (let line of markdown.split("\n")) {
		// Trim line to remove any trailing whitespace including \r on Windows
		line = line.trim();
		let match = line.match(/^(#{1,6})\s+(.+)$/); // Look for heading in line

		if (match) {
			// Add match to heading array
			const text = match[2].trim();
			headings.push({ text, id });
			id++;
		}
	}

	return headings;
}

/**
 * Search bar focus handler - called when searchbar receives focus
 */
function searchbarFocus() {
	const searchPopup = document.getElementById("searchPopup");
	searchPopup.classList.add("visible");
}

/**
 * Search bar blur handler - called when searchbar loses focus
 */
function searchbarBlur() {
	const searchPopup = document.getElementById("searchPopup");
	const searchInput = document.getElementById("searchInput");
	// Use setTimeout to allow click events on popup items to fire before hiding
	setTimeout(() => {
		searchPopup.classList.remove("visible");
		searchInput.value = "";

		// On mobile collapse the search bar back to icon-only
		try {
			if (window.innerWidth <= 768) {
				const bar = document.querySelector('.header-search-bar');
				if (bar) {
					bar.classList.remove('expanded');
					bar.classList.add('mobile-collapsed');
				}
			}
		} catch (e) {
			// ignore
		}
	}, 200);
}

// Mobile search icon toggle: collapse to icon, expand on click to reveal input
document.addEventListener('DOMContentLoaded', () => {
	const searchBar = document.querySelector('.header-search-bar');
	const searchIcon = document.querySelector('.header-search-bar .search-icon');
	const searchInput = document.getElementById('searchInput');

	function applyMobileState() {
		if (!searchBar) return;
		if (window.innerWidth <= 768) {
			// default collapsed state on mobile
			if (!searchBar.classList.contains('expanded')) {
				searchBar.classList.add('mobile-collapsed');
			}
		} else {
			// remove mobile-only classes on larger screens
			searchBar.classList.remove('mobile-collapsed');
			searchBar.classList.remove('expanded');
		}
	}

	applyMobileState();

	window.addEventListener('resize', applyMobileState);

	if (searchIcon) {
		searchIcon.addEventListener('click', (e) => {
			if (window.innerWidth > 768) return; // default behavior on desktop

			const bar = searchIcon.closest('.header-search-bar');
			if (!bar) return;

			if (bar.classList.contains('expanded')) {
				// collapse
				bar.classList.remove('expanded');
				bar.classList.add('mobile-collapsed');
				if (searchInput) searchInput.blur();
			} else {
				// expand and focus
				bar.classList.remove('mobile-collapsed');
				bar.classList.add('expanded');
				// slight delay to allow CSS transition
				setTimeout(() => {
					if (searchInput) {
						searchInput.focus();
						// open popup too
						const popup = document.getElementById('searchPopup');
						if (popup) popup.classList.add('visible');
					}
				}, 160);
			}
		});
	}
});

// Debounce timer for search
let searchDebounceTimer = null;

/**
 * Search bar searching logic - searches through all markdown files
 * and displays results grouped by category
 */
async function searchbarSearch() {
	const searchInput = document.getElementById("searchInput");
	const searchPopup = document.getElementById("searchPopup");
	const searchResults = searchPopup.querySelector(".search-results");
	const searchTerm = searchInput.value.toLowerCase().trim();

	// Clear previous timeout
	if (searchDebounceTimer) {
		clearTimeout(searchDebounceTimer);
	}

	// If search term is empty, clear results
	if (searchTerm.length < 2) {
		searchResults.innerHTML = '<li class="search-no-results">Type at least 2 characters to search...</li>';
		return;
	}

	// Debounce: wait 300ms after user stops typing
	searchDebounceTimer = setTimeout(async () => {
		searchResults.innerHTML = '<li class="search-loading">Searching...</li>';

		const resultsByCategory = {};

		// Recursive function to search through the content structure
		const searchInStructure = async (items, parentCategory = "General") => {
			for (const item of items) {
				if (item.type === "page" || (item.type === "category" && item.path)) {
					// This item has a markdown file to search
					try {
						const markdown = await converter.loadMarkdown( item.path);
						const lowerMarkdown = markdown.toLowerCase();

						if (lowerMarkdown.includes(searchTerm)) {
							// Find all matches and create snippets
							const snippets = findSnippets(markdown, searchTerm);

							if (!resultsByCategory[parentCategory]) {
								resultsByCategory[parentCategory] = [];
							}

							resultsByCategory[parentCategory].push({
								name: item.name,
								path: item.path,
								snippets: snippets,
							});
						}
					} catch (err) {
						console.warn(`Failed to search in ${item.path}:`, err);
					}
				}

				// If it's a category, search its children
				if (item.type === "category" && item.children) {
					// Use this category's name as the parent for its children
					await searchInStructure(item.children, item.name);
				}
			}
		};

		await searchInStructure(browser.contentStructure);

		// Generate and display results HTML
		displaySearchResults(resultsByCategory, searchResults, searchTerm);
	}, 300);
}

/**
 * Find snippets of text around search matches
 * @param {string} markdown - The full markdown content
 * @param {string} searchTerm - The search term to find
 * @returns {Array} - Array of snippet objects with text and search term info
 */
function findSnippets(markdown, searchTerm) {
	const snippets = [];
	const lowerMarkdown = markdown.toLowerCase();
	const lowerSearchTerm = searchTerm.toLowerCase();
	let startIndex = 0;
	const maxSnippets = 3; // Limit number of snippets per file

	while (snippets.length < maxSnippets) {
		const index = lowerMarkdown.indexOf(lowerSearchTerm, startIndex);
		if (index === -1) break;

		// Get context around the match (50 chars before and after)
		const snippetStart = Math.max(0, index - 50);
		const snippetEnd = Math.min(markdown.length, index + searchTerm.length + 50);
		let snippet = markdown.substring(snippetStart, snippetEnd);

		// Get the actual matched text (preserving case)
		const matchedText = markdown.substring(index, index + searchTerm.length);

		// Clean up the snippet - remove markdown syntax and extra whitespace
		snippet = snippet
			.replace(/[#*_`\[\]]/g, "")
			.replace(/\s+/g, " ")
			.trim();

		// Add ellipsis if we didn't start/end at boundaries
		if (snippetStart > 0) snippet = "..." + snippet;
		if (snippetEnd < markdown.length) snippet = snippet + "...";

		snippets.push({
			text: snippet,
			searchTerm: searchTerm,
			matchedText: matchedText,
		});
		startIndex = index + searchTerm.length;
	}

	return snippets;
}

/**
 * Display search results in the popup
 * @param {Object} resultsByCategory - Results grouped by category
 * @param {HTMLElement} container - The container to display results in
 * @param {string} searchTerm - The search term for highlighting
 */
function displaySearchResults(resultsByCategory, container, searchTerm) {
	const categories = Object.keys(resultsByCategory);

	if (categories.length === 0) {
		container.innerHTML = '<li class="search-no-results">No results found</li>';
		return;
	}

	let html = "";

	for (const category of categories) {
		html += `<li class="search-category-header">${category}</li>`;

		for (const result of resultsByCategory[category]) {
			html += `<li class="search-result-item">`;
			html += `<div class="search-result-title-row" data-path="${result.path}">`;
			html += `<span class="search-result-title">${result.name}</span>`;
			html += `</div>`;

			// Show all snippets with highlighted search terms - each clickable
			if (result.snippets.length > 0) {
				html += `<div class="search-result-snippets">`;
				for (const snippet of result.snippets) {
					const highlightedSnippet = highlightSearchTerm(snippet.text, searchTerm);
					html += `<div class="search-result-snippet" data-path="${result.path}" data-search="${escapeHtml(snippet.searchTerm)}">`;
					html += `<span class="snippet-text">${highlightedSnippet}</span>`;
					html += `</div>`;
				}
				html += `</div>`;
			}

			html += `</li>`;
		}
	}

	container.innerHTML = html;

	// Add click handlers to search results
	setupSearchResultClickHandlers();
}

/**
 * Escape HTML special characters to prevent XSS
 * @param {string} text - Text to escape
 * @returns {string} - Escaped text
 */
function escapeHtml(text) {
	const div = document.createElement("div");
	div.textContent = text;
	return div.innerHTML;
}

/**
 * Highlight the search term in a snippet
 * @param {string} snippet - The text snippet
 * @param {string} searchTerm - The term to highlight
 * @returns {string} - HTML with highlighted term
 */
function highlightSearchTerm(snippet, searchTerm) {
	const regex = new RegExp(`(${escapeRegExp(searchTerm)})`, "gi");
	return snippet.replace(regex, "<mark>$1</mark>");
}

/**
 * Escape special regex characters
 * @param {string} string - String to escape
 * @returns {string} - Escaped string
 */
function escapeRegExp(string) {
	return string.replace(/[.*+?^${}()|[\]\\]/g, "\\$&");
}

/**
 * Set up click handlers for search result items
 */
function setupSearchResultClickHandlers() {
	const titleRows = document.querySelectorAll(".search-result-title-row");
	const snippetItems = document.querySelectorAll(".search-result-snippet");

	// Handler for clicking on page title - navigates to page
	titleRows.forEach((item) => {
		item.addEventListener("click", async (e) => {
			e.stopPropagation();
			await navigateToSearchResult(item.dataset.path, null);
		});
	});

	// Handler for clicking on snippet - navigates to page and scrolls to match
	snippetItems.forEach((item) => {
		item.addEventListener("click", async (e) => {
			e.stopPropagation();
			const searchTerm = item.dataset.search;
			await navigateToSearchResult(item.dataset.path, searchTerm);
		});
	});
}

/**
 * Navigate to a search result and optionally scroll to the matching text
 * @param {string} path - Path to the markdown file
 * @param {string|null} searchTerm - The search term to scroll to, or null to just navigate
 */
async function navigateToSearchResult(path, searchTerm) {
	if (!path) return;

	// Find and select the corresponding topic button
	const targetButton = document.querySelector(`[data-path="${path}"]`);

	// Deselect current selections
	if (currentlySelectedTopic) {
		currentlySelectedTopic.classList.remove("topic-selected");
		currentlySelectedTopic.classList.add("topic-unselected");
	}
	if (currentlySelectedCategory) {
		currentlySelectedCategory.classList.remove("topic-category-button-selected");
		currentlySelectedCategory = null;
	}

	// If the button exists in the DOM, select it
	if (targetButton) {
		targetButton.classList.remove("topic-unselected");
		targetButton.classList.add("topic-selected");
		currentlySelectedTopic = targetButton;
	}

	// Load and display the content
	const md = await converter.loadMarkdown( path);
	currentMarkdownContent = md;
	const html = converter.convert(md, path);
	preview.innerHTML = html;

	const rightPanelHeader = document.getElementById("right-panel-header");
	const headings = await getMarkdownHeaders( path);
	rightPanelHeader.innerHTML = await generateHtmlRightHeader(headings);
	setupRightPanelListeners(rightPanelHeader);
	setupEndButtonListeners();

	// Save state
	saveContentStructureState();

	// Close the search popup
	const searchPopup = document.getElementById("searchPopup");
	searchPopup.classList.remove("visible");
	document.getElementById("searchInput").value = "";

	// If we have a search term, find and scroll to it
	if (searchTerm) {
		// Wait for DOM to update
		setTimeout(() => {
			scrollToTextInContent(searchTerm);
		}, 100);
	}
}

/**
 * Find and scroll to text content in the display window, highlighting it
 * @param {string} searchTerm - The text to find and scroll to
 */
function scrollToTextInContent(searchTerm) {
	const container = document.querySelector(".display-window");
	const markdownContainer = document.getElementById("markdown-container");

	if (!markdownContainer || !container) return;

	// Use TreeWalker to find text nodes containing the search term
	const walker = document.createTreeWalker(markdownContainer, NodeFilter.SHOW_TEXT, {
		acceptNode: function (node) {
			if (node.textContent.toLowerCase().includes(searchTerm.toLowerCase())) {
				return NodeFilter.FILTER_ACCEPT;
			}
			return NodeFilter.FILTER_SKIP;
		},
	});

	const textNode = walker.nextNode();
	if (textNode && textNode.parentElement) {
		const element = textNode.parentElement;

		// Scroll the element into view
		const rect = element.getBoundingClientRect();
		const containerRect = container.getBoundingClientRect();
		const scrollTop = container.scrollTop + (rect.top - containerRect.top) - 100; // 100px offset from top

		container.scrollTo({ top: scrollTop, behavior: "smooth" });

		// Highlight the element temporarily
		element.classList.add("search-highlight-flash");
		setTimeout(() => {
			element.classList.remove("search-highlight-flash");
		}, 2000);
	}
}

/**
 * Toggle between light mode and dark mode
 */
function lightDarkModeToggle() {
	const html = document.documentElement;
	const toggleButton = document.getElementById("lightDarkToggle");
	const toggleIcon = toggleButton.querySelector(".icon");
	const toggleText = toggleButton.querySelector(".icon-text");

	html.classList.toggle("dark-mode"); // Change page appearance

	// Toggle mode switch button content (only update src and text, preserve DOM for smooth transition)
	if (html.classList.contains("dark-mode")) {
		toggleIcon.src = "../../assets/img/light.svg";
		toggleText.textContent = "Light Mode";
		localStorage.setItem("theme", "dark");
	} else {
		toggleIcon.src = "../../assets/img/dark.svg";
		toggleText.textContent = "Dark Mode";
		localStorage.setItem("theme", "light");
	}
}

/**
 * Generate the header list with the buttons to skip to certain header in the right hand side bar
 * @param {HTMLHeadElement[]} headings Array containing the headers
 * @returns The generated html
 */
function generateHtmlRightHeader(headings) {
	let html = '<ul class="right-bar-list">';
	headings.forEach((heading) => {
		html += `<li class="right-bar-items"><a href="#${heading.id}">${heading.text}</a></li>`;
	});
	html += "</ul>";
	return html;
}

/**
 * Set up event listeners for right panel navigation links to scroll within the display window
 * @param {HTMLElement} rightPanelHeader - The right panel header element containing the links
 */
function setupRightPanelListeners(rightPanelHeader) {
	const links = rightPanelHeader.querySelectorAll("a");
	links.forEach((link) => {
		link.addEventListener("click", (e) => {
			e.preventDefault();
			const targetId = link.getAttribute("href").substring(1);
			const targetElement = document.getElementById(targetId);
			if (targetElement) {
				const container = document.querySelector(".display-window");
				const rect = targetElement.getBoundingClientRect();
				const containerRect = container.getBoundingClientRect();
				const scrollTop = container.scrollTop + (rect.top - containerRect.top);
				container.scrollTo({ top: scrollTop, behavior: "smooth" });
			}
		});
	});
}

/**
 * Recursively find a category by name in the content structure
 * @param {Array} structure - The content structure to search
 * @param {string} name - The name of the category to find
 * @returns {Object|null} - The category item or null if not found
 */
function findCategoryByName(structure, name) {
	for (const item of structure) {
		if (item.type === "category" && item.name === name) {
			return item;
		}
		if (item.children) {
			const found = findCategoryByName(item.children, name);
			if (found) return found;
		}
	}
	return null;
}

/**
 * Generate HTML for a category's children
 * @param {Object} categoryItem - The category item whose children to generate HTML for
 * @returns {string} - HTML string for the children
 */
function generateCategoryChildrenHTML(categoryItem) {
	let html = "";

	const traverse = (items, parentIsCategoryButton = false) => {
		items.forEach((item) => {
			if (item.type === "page") {
				// Generate HTML for individual pages
				const categoryClass = parentIsCategoryButton ? "topic-under-category" : "";
				html += `<p class="topic-unselected topic-button ${categoryClass}" id="topic-button-${item.name}" data-path="${item.path}">${item.name}</p>`;
			} else if (item.type === "category") {
				// Check if this category is a category button (has path property)
				const isCategoryButton = !!item.path;

				// Generate HTML for all categories - let DOM state management handle visibility
				if (item.path) {
					html += `<p class="topic-category-button-unselected topic-category-button" id="topic-category-topic-${item.name}">${item.name}</p>`;
				} else {
					html += `<p class="topic-category" id="topic-category-${item.name}">${item.name}</p>`;
				}

				// Always include children in HTML generation - DOM will control visibility
				if (item.children) {
					traverse(item.children, isCategoryButton);
				}
			}
		});
	};

	if (categoryItem.children) {
		traverse(categoryItem.children, !!categoryItem.path);
	}

	return html;
}

/**
 * Save the current content structure state and selected page to localStorage
 */
function saveContentStructureState() {
	try {
		const stateToSave = {
			contentStructure: browser.contentStructure,
			selectedPage: currentlySelectedTopic ? currentlySelectedTopic.dataset.path : null,
			selectedCategory: currentlySelectedCategory ? currentlySelectedCategory.textContent : null,
		};
		localStorage.setItem("droneContentStructure", JSON.stringify(stateToSave));
	} catch (error) {
		console.warn("Failed to save content structure state to localStorage:", error);
	}
}

/**
 * Load the content structure state from localStorage
 * @returns {Object|null} - The saved state object or null if not found
 */
function loadContentStructureState() {
	try {
		const saved = localStorage.getItem("droneContentStructure");
		if (!saved) return null;

		const parsed = JSON.parse(saved);

		// Handle legacy format (just array) vs new format (object with structure + page)
		if (Array.isArray(parsed)) {
			return { contentStructure: parsed, selectedPage: null, selectedCategory: null };
		}

		return parsed;
	} catch (error) {
		console.warn("Failed to load content structure state from localStorage:", error);
		return null;
	}
}

/**
 * Update the collapsed state of a category in the content structure
 * @param {Array} structure - The content structure to update
 * @param {string} categoryName - The name of the category to update
 * @param {boolean} collapsed - The new collapsed state
 */
function updateCategoryCollapsedState(structure, categoryName, collapsed) {
	for (const item of structure) {
		if (item.type === "category" && item.name === categoryName) {
			item.collapsed = collapsed;
			return true;
		}
		if (item.children) {
			if (updateCategoryCollapsedState(item.children, categoryName, collapsed)) {
				return true;
			}
		}
	}
	return false;
}

/**
 * Restore the selected page from saved state
 * @param {string} savedPagePath - The path of the previously selected page
 * @param {string} savedCategoryName - The name of the previously selected category
 */
async function restoreSelectedPage(savedPagePath, savedCategoryName) {
	if (savedPagePath) {
		// Find and select the page button
		const pageButton = document.querySelector(`[data-path="${savedPagePath}"]`);
		if (pageButton) {
			// Deselect any currently selected items
			if (currentlySelectedTopic) {
				currentlySelectedTopic.classList.remove("topic-selected");
				currentlySelectedTopic.classList.add("topic-unselected");
			}
			if (currentlySelectedCategory) {
				currentlySelectedCategory.classList.remove("topic-category-button-selected");
				currentlySelectedCategory = null;
			}

			// Select the saved page
			pageButton.classList.remove("topic-unselected");
			pageButton.classList.add("topic-selected");
			currentlySelectedTopic = pageButton;

			// Load the content
			const md = await converter.loadMarkdown( savedPagePath);
			currentMarkdownContent = md;
			const html = converter.convert(md, savedPagePath);
			preview.innerHTML = html;

			const rightPanelHeader = document.getElementById("right-panel-header");
			const headings = await getMarkdownHeaders( savedPagePath);
			rightPanelHeader.innerHTML = await generateHtmlRightHeader(headings);
			setupRightPanelListeners(rightPanelHeader);
			setupEndButtonListeners();
		}
	} else if (savedCategoryName) {
		// Find and select the category button
		const categoryButton = document.querySelector(`#topic-category-topic-${savedCategoryName.replace(/[^a-zA-Z0-9\s]/g, "")}`);
		if (categoryButton && categoryButton.classList.contains("topic-category-button-expanded")) {
			categoryButton.classList.add("topic-category-button-selected");
			currentlySelectedCategory = categoryButton;

			// Load the category content if it has a path
			const categoryItem = findCategoryByName(browser.contentStructure, savedCategoryName);
			if (categoryItem && categoryItem.path) {
				const md = await converter.loadMarkdown( categoryItem.path);
				currentMarkdownContent = md;
				const html = converter.convert(md, categoryItem.path);
				preview.innerHTML = html;

				const rightPanelHeader = document.getElementById("right-panel-header");
				const headings = await getMarkdownHeaders( categoryItem.path);
				rightPanelHeader.innerHTML = await generateHtmlRightHeader(headings);
				setupRightPanelListeners(rightPanelHeader);
				setupEndButtonListeners();
			}
		}
	}
}

/**
 * Merge saved content structure states with fresh structure
 * @param {Array} freshStructure - The freshly loaded content structure
 * @param {Array} savedStructure - The saved content structure with user states
 * @returns {Array} - The merged content structure
 */
function mergeContentStructureStates(freshStructure, savedStructure) {
	const mergeItems = (fresh, saved) => {
		return fresh.map((freshItem) => {
			// Find corresponding item in saved structure
			const savedItem = saved.find((s) => s.name === freshItem.name && s.type === freshItem.type);

			const mergedItem = { ...freshItem };

			// If we found a saved item and it's a category, use its collapsed state
			if (savedItem && freshItem.type === "category") {
				mergedItem.collapsed = savedItem.collapsed;
			}

			// Recursively merge children if they exist
			if (freshItem.children && savedItem && savedItem.children) {
				mergedItem.children = mergeItems(freshItem.children, savedItem.children);
			}

			return mergedItem;
		});
	};

	return mergeItems(freshStructure, savedStructure);
}

/**
 * Remove all DOM elements that belong to a specific category's children
 * @param {HTMLElement} categoryButton - The category button element
 * @param {Object} categoryItem - The category item from the data structure
 */
function removeCategoryChildren(categoryButton, categoryItem) {
	const elementsToRemove = [];

	const collectElementIds = (items) => {
		items.forEach((item) => {
			if (item.type === "page") {
				elementsToRemove.push(`topic-button-${item.name}`);
			} else if (item.type === "category") {
				if (item.path) {
					elementsToRemove.push(`topic-category-topic-${item.name}`);
				} else {
					elementsToRemove.push(`topic-category-${item.name}`);
				}
				if (item.children) {
					collectElementIds(item.children);
				}
			}
		});
	};

	if (categoryItem.children) {
		collectElementIds(categoryItem.children);
	}

	// Apply fade-out class to all elements
	elementsToRemove.forEach((id) => {
		const element = document.getElementById(id);
		if (element) {
			element.classList.add("fade-out");
		}
	});

	// Remove elements after animation completes
	setTimeout(() => {
		elementsToRemove.forEach((id) => {
			const element = document.getElementById(id);
			if (element) {
				element.remove();
			}
		});
	}, 200); // Match the CSS animation duration (0.2s)
}

/**
 * Set up event listeners for newly added elements without affecting existing ones
 */
function setupEventListenersForNewElements() {
	// Get only the buttons that don't already have event listeners
	const newTopicButtons = document.querySelectorAll(".topic-button:not([data-has-listener])");
	const newCategoryButtons = document.querySelectorAll(".topic-category-button:not([data-has-listener])");

	// Set up event listeners for new category buttons
	newCategoryButtons.forEach((button) => {
		button.setAttribute("data-has-listener", "true");

		// Add arrow if it doesn't exist
		if (!button.querySelector(".category-arrow")) {
			const arrow = document.createElement("span");
			arrow.classList.add("category-arrow");
			arrow.innerHTML = '<img src="../../assets/img/arrow.svg" class="category-arrow" alt="arrow" width="15" height="15">';
			button.appendChild(arrow);
		}

		// Initialize state
		button.dataset.toggled = "false";

		button.addEventListener("click", async () => {
			const isToggled = button.dataset.toggled === "true";

			// Deselect previously selected topic button (O(1))
			if (currentlySelectedTopic) {
				currentlySelectedTopic.classList.remove("topic-selected");
				currentlySelectedTopic.classList.add("topic-unselected");
				currentlySelectedTopic = null;
			}

			// Toggle this category independently
			if (isToggled) {
				// Collapse this category
				button.classList.remove("topic-category-button-expanded");
				button.classList.add("topic-category-button-collapsed");
				button.dataset.toggled = "false";
				if (currentlySelectedCategory === button) {
					currentlySelectedCategory.classList.remove("topic-category-button-selected");
					currentlySelectedCategory = null;
				}
				// Update content structure and save to localStorage
				updateCategoryCollapsedState(browser.contentStructure, button.textContent, true);
				saveContentStructureState();
				onDeToggle(button);
			} else {
				// Expand this category
				button.classList.remove("topic-category-button-collapsed");
				button.classList.add("topic-category-button-expanded");
				button.dataset.toggled = "true";
				// Update content structure and save to localStorage
				updateCategoryCollapsedState(browser.contentStructure, button.textContent, false);
				saveContentStructureState();
				await onToggle(button);
			}
		});
	});

	// Set up event listeners for new topic buttons
	newTopicButtons.forEach((button) => {
		button.setAttribute("data-has-listener", "true");

		button.addEventListener("click", async () => {
			if (currentlySelectedTopic) {
				currentlySelectedTopic.classList.remove("topic-selected");
				currentlySelectedTopic.classList.add("topic-unselected");
			}

			// Clear the currently selected category for content display tracking
			if (currentlySelectedCategory) {
				currentlySelectedCategory.classList.remove("topic-category-button-selected");
				currentlySelectedCategory = null;
			}

			button.classList.remove("topic-unselected");
			button.classList.add("topic-selected");
			currentlySelectedTopic = button;

			const md = await converter.loadMarkdown( button.dataset.path);
			currentMarkdownContent = md; // for copy button
			const html = converter.convert(md, button.dataset.path);
			preview.innerHTML = html;

			const rightPanelHeader = document.getElementById("right-panel-header");
			const headings = await getMarkdownHeaders( button.dataset.path);
			rightPanelHeader.innerHTML = await generateHtmlRightHeader(headings);
			setupRightPanelListeners(rightPanelHeader);
			setupEndButtonListeners();
		});
	});
}

/**
 * Set up event listeners for Previous/Next navigation buttons
 */
function setupEndButtonListeners() {
	const endPreviousButtons = document.querySelectorAll(".markdown-end-previous");
	const endNextButtons = document.querySelectorAll(".markdown-end-next");

	endPreviousButtons.forEach((button) => {
		button.addEventListener("click", async () => {
			const path = button.dataset.path;
			if (path) {
				// Find and select the corresponding topic button
				const targetButton = document.querySelector(`[data-path="${path}"]`);
				if (targetButton) {
					// Deselect current topic
					if (currentlySelectedTopic) {
						currentlySelectedTopic.classList.remove("topic-selected");
						currentlySelectedTopic.classList.add("topic-unselected");
					}

					// Clear the currently selected category
					if (currentlySelectedCategory) {
						currentlySelectedCategory.classList.remove("topic-category-button-selected");
						currentlySelectedCategory = null;
					}

					// Select the new topic button
					targetButton.classList.remove("topic-unselected");
					targetButton.classList.add("topic-selected");
					currentlySelectedTopic = targetButton;

					// Load and display the content
					const md = await converter.loadMarkdown( path);
					currentMarkdownContent = md;
					const html = converter.convert(md, path);
					preview.innerHTML = html;

					const rightPanelHeader = document.getElementById("right-panel-header");
					const headings = await getMarkdownHeaders( path);
					rightPanelHeader.innerHTML = await generateHtmlRightHeader(headings);
					setupRightPanelListeners(rightPanelHeader);

					// Scroll to top of display window instantly
					const displayWindow = document.querySelector(".display-window");
					if (displayWindow) {
						displayWindow.scrollTop = 0;
					}

					// Re-setup event listeners for new end buttons
					setupEndButtonListeners();
				}
			}
		});
	});

	endNextButtons.forEach((button) => {
		button.addEventListener("click", async () => {
			const path = button.dataset.path;
			if (path) {
				// Find and select the corresponding topic button
				const targetButton = document.querySelector(`[data-path="${path}"]`);
				if (targetButton) {
					// Deselect current topic
					if (currentlySelectedTopic) {
						currentlySelectedTopic.classList.remove("topic-selected");
						currentlySelectedTopic.classList.add("topic-unselected");
					}

					// Clear the currently selected category
					if (currentlySelectedCategory) {
						currentlySelectedCategory.classList.remove("topic-category-button-selected");
						currentlySelectedCategory = null;
					}

					// Select the new topic button
					targetButton.classList.remove("topic-unselected");
					targetButton.classList.add("topic-selected");
					currentlySelectedTopic = targetButton;

					// Load and display the content
					const md = await converter.loadMarkdown( path);
					currentMarkdownContent = md;
					const html = converter.convert(md, path);
					preview.innerHTML = html;

					const rightPanelHeader = document.getElementById("right-panel-header");
					const headings = await getMarkdownHeaders( path);
					rightPanelHeader.innerHTML = await generateHtmlRightHeader(headings);
					setupRightPanelListeners(rightPanelHeader);

					// Scroll to top of display window instantly
					const displayWindow = document.querySelector(".display-window");
					if (displayWindow) {
						displayWindow.scrollTop = 0;
					}

					// Save the selected page state
					saveContentStructureState();

					// Re-setup event listeners for new end buttons
					setupEndButtonListeners();
				}
			}
		});
	});
}

/**
 * Set up click handlers for all buttons - called after each refresh
 */
async function setupEventListeners() {
	// Get fresh references to all topic buttons and category buttons
	const topicButtons = document.querySelectorAll(".topic-button");
	const topicCategoryButtons = document.querySelectorAll(".topic-category-button");

	/**
	 * Set up click handlers for topic category buttons
	 */
	topicCategoryButtons.forEach((button) => {
		button.setAttribute("data-has-listener", "true");
		// Load the current state from content browser and apply it
		const categoryItem = findCategoryByName(browser.contentStructure, button.textContent);

		if (!button.querySelector(".category-arrow")) {
			const arrow = document.createElement("span");
			arrow.classList.add("category-arrow");
			arrow.innerHTML = '<img src="../../assets/img/arrow.svg" class="category-arrow" alt="arrow" width="15" height="15">';
			button.appendChild(arrow);
		}

		if (categoryItem) {
			const isExpanded = !categoryItem.collapsed;

			// Check if children exist in DOM to determine actual UI state
			let hasChildrenInDOM = false;
			if (categoryItem.children && categoryItem.children.length > 0) {
				const firstChild = categoryItem.children[0];
				const firstChildId = firstChild.type === "page" ? `topic-button-${firstChild.name}` : firstChild.path ? `topic-category-topic-${firstChild.name}` : `topic-category-${firstChild.name}`;
				hasChildrenInDOM = document.getElementById(firstChildId) !== null;
			}

			// Set button state based on whether children are visible in DOM
			button.dataset.toggled = hasChildrenInDOM.toString();

			// Apply visual state based on DOM state (not data structure)
			if (hasChildrenInDOM) {
				button.classList.remove("topic-category-button-collapsed");
				button.classList.add("topic-category-button-expanded");

				// Set arrow rotation for expanded state without animation on initial load
				const arrow = button.querySelector(".category-arrow");
				if (arrow) {
					arrow.style.transform = "rotate(90deg)";
					arrow.style.transition = "none"; // No animation on initial load
					// Re-enable animation after a brief delay
					setTimeout(() => {
						arrow.style.transition = "transform 0.3s ease";
					}, 10);
				}
			} else {
				button.classList.remove("topic-category-button-expanded");
				button.classList.add("topic-category-button-collapsed");

				// Set arrow rotation for collapsed state without animation on initial load
				const arrow = button.querySelector(".category-arrow");
				if (arrow) {
					arrow.style.transform = "rotate(0deg)";
					arrow.style.transition = "none"; // No animation on initial load
					// Re-enable animation after a brief delay
					setTimeout(() => {
						arrow.style.transition = "transform 0.3s ease";
					}, 10);
				}
			}
		} else {
			// Fallback: Initialize as not toggled if category not found
			button.dataset.toggled = "false";
		}

		button.addEventListener("click", async () => {
			// Check current toggle state before making changes
			const isToggled = button.dataset.toggled === "true";

			// Deselect previously selected topic button (O(1))
			if (currentlySelectedTopic) {
				currentlySelectedTopic.classList.remove("topic-selected");
				currentlySelectedTopic.classList.add("topic-unselected");
				currentlySelectedTopic = null;
			}

			// Toggle this category independently
			if (isToggled) {
				// Collapse this category
				button.classList.remove("topic-category-button-expanded");
				button.classList.add("topic-category-button-collapsed");
				button.dataset.toggled = "false";
				if (currentlySelectedCategory === button) {
					currentlySelectedCategory.classList.remove("topic-category-button-selected");
					currentlySelectedCategory = null;
				}
				// Update content structure and save to localStorage
				updateCategoryCollapsedState(browser.contentStructure, button.textContent, true);
				saveContentStructureState();
				onDeToggle(button);
			} else {
				// Expand this category
				button.classList.remove("topic-category-button-collapsed");
				button.classList.add("topic-category-button-expanded");
				button.dataset.toggled = "true";
				// Update content structure and save to localStorage
				updateCategoryCollapsedState(browser.contentStructure, button.textContent, false);
				saveContentStructureState();
				await onToggle(button);
			}
		});
	});

	/**
	 * Set up click handlers for individual topic buttons after category buttons
	 */
	topicButtons.forEach((button) => {
		button.setAttribute("data-has-listener", "true");

		button.addEventListener("click", async () => {
			if (currentlySelectedTopic) {
				currentlySelectedTopic.classList.remove("topic-selected");
				currentlySelectedTopic.classList.add("topic-unselected");
			}

			// Clear the currently selected category for content display tracking
			if (currentlySelectedCategory) {
				currentlySelectedCategory.classList.remove("topic-category-button-selected");
				currentlySelectedCategory = null;
			}

			// Select the clicked topic button
			button.classList.remove("topic-unselected");
			button.classList.add("topic-selected");
			currentlySelectedTopic = button;

			const md = await converter.loadMarkdown(button.dataset.path);
			currentMarkdownContent = md; // for copy button
			const html = converter.convert(md, button.dataset.path);
			preview.innerHTML = html;

			const rightPanelHeader = document.getElementById("right-panel-header");
			const headings = await getMarkdownHeaders( button.dataset.path);
			rightPanelHeader.innerHTML = await generateHtmlRightHeader(headings);
			setupRightPanelListeners(rightPanelHeader);
			setupEndButtonListeners();

			// Save the selected page state
			saveContentStructureState();
		});
	});

	// Select first topic-button on load if none selected and no category is expanded
	if (topicButtons.length > 0 && !currentlySelectedTopic && !currentlySelectedCategory) {
		topicButtons[0].classList.remove("topic-unselected");
		topicButtons[0].classList.add("topic-selected");
		currentlySelectedTopic = topicButtons[0];

		const md = await converter.loadMarkdown(topicButtons[0].dataset.path);
		currentMarkdownContent = md; // for copy button
		const html = converter.convert(md, topicButtons[0].dataset.path);
		preview.innerHTML = html;

		const rightPanelHeader = document.getElementById("right-panel-header");
		const headings = await getMarkdownHeaders(topicButtons[0].dataset.path);
		rightPanelHeader.innerHTML = await generateHtmlRightHeader(headings);
		setupRightPanelListeners(rightPanelHeader);
		setupEndButtonListeners();
	}

	/**
	 * Set up click handlers for Previous/Next navigation buttons
	 */
	setupEndButtonListeners();
}

/**
 * Handle category expansion - rotates arrow to indicate expanded state
 * @param {HTMLElement} button - The category button that was toggled
 */
async function onToggle(button) {
	const arrow = button.querySelector(".category-arrow");
	if (arrow) {
		arrow.style.transform = "rotate(90deg)"; // Rotate arrow to point down
		arrow.style.transition = "transform 0.3s ease";
	}

	// Find category but don't modify its collapsed state
	const categoryItem = findCategoryByName(browser.contentStructure, button.textContent);
	if (categoryItem) {
		// If this category has a path, load its markdown content and mark as selected
		if (categoryItem.path) {
			// Clear previous category selection
			if (currentlySelectedCategory && currentlySelectedCategory !== button) {
				currentlySelectedCategory.classList.remove("topic-category-button-selected");
			}

			// Mark this category as selected for content display
			button.classList.add("topic-category-button-selected");
			currentlySelectedCategory = button;

			const md = await converter.loadMarkdown(categoryItem.path);
			currentMarkdownContent = md; // for copy button
			const html = converter.convert(md, categoryItem.path);
			preview.innerHTML = html;

			const rightPanelHeader = document.getElementById("right-panel-header");
			const headings = await getMarkdownHeaders( categoryItem.path);
			rightPanelHeader.innerHTML = await generateHtmlRightHeader(headings);
			setupRightPanelListeners(rightPanelHeader);
			setupEndButtonListeners();

			// Save the selected category state
			saveContentStructureState();
		}

		// Check if children already exist in the DOM to prevent duplicates
		let hasExistingChildren = false;
		if (categoryItem.children && categoryItem.children.length > 0) {
			const firstChild = categoryItem.children[0];
			const firstChildId = firstChild.type === "page" ? `topic-button-${firstChild.name}` : firstChild.path ? `topic-category-topic-${firstChild.name}` : `topic-category-${firstChild.name}`;
			hasExistingChildren = document.getElementById(firstChildId) !== null;
		}

		// Only insert children if they don't already exist
		if (!hasExistingChildren) {
			// Generate HTML for just this category's children
			const childrenHTML = generateCategoryChildrenHTML(categoryItem);

			// Insert the children HTML after this button
			const nextSibling = button.nextElementSibling;
			const tempDiv = document.createElement("div");
			tempDiv.innerHTML = childrenHTML;

			// Insert all children after the button
			while (tempDiv.firstChild) {
				if (nextSibling) {
					browserContainer.insertBefore(tempDiv.firstChild, nextSibling);
				} else {
					browserContainer.appendChild(tempDiv.firstChild);
				}
			}

			// Set up event listeners for the newly added elements
			setupEventListenersForNewElements();
		}
	}
}

/**
 * Handle category collapse - rotates arrow back to indicate collapsed state
 * @param {HTMLElement} button - The category button that was toggled
 */
function onDeToggle(button) {
	const arrow = button.querySelector(".category-arrow");
	if (arrow) {
		arrow.style.transform = "rotate(0deg)"; // Rotate arrow back to point right
		arrow.style.transition = "transform 0.3s ease";
	}

	// Find category but don't modify its collapsed state - only manage DOM
	const categoryItem = findCategoryByName(browser.contentStructure, button.textContent);
	if (categoryItem) {
		// Remove all children elements that belong to this category from DOM only
		removeCategoryChildren(button, categoryItem);
	}
}

/* 
Trigger the copy button to copy the current page content into the clipboard
**/
async function copyButtonTrigger() {
	try {
		await navigator.clipboard.writeText(currentMarkdownContent);
	} catch (err) {
		console.error("Failed to copy text: ", err);
	}
}

/**
 * Copy code from a formatted code block to clipboard
 * @param {HTMLElement} button - The copy button that was clicked
 */
async function copyCodeBlock(button) {
	const codeBlock = button.closest(".markdown-formatted-code");
	const codeIndex = parseInt(codeBlock.getAttribute("data-code-index"));
	const code = converter.codeBlocks[codeIndex];
	const textSpan = button.querySelector(".code-copy-text");

	try {
		await navigator.clipboard.writeText(code);
		// Show feedback
		setTimeout(() => {
			textSpan.textContent = "Copy";
		}, 2000);
	} catch (err) {
		console.error("Failed to copy code: ", err);
		textSpan.textContent = "Failed";
		setTimeout(() => {
			textSpan.textContent = "Copy";
		}, 2000);
	}
}

// Refreshes MarkDown-Container and Topic-Container
async function refresh() {
	// Populate container with the generated topics
	browserContainer.innerHTML = browser.generateTopicsHTML();
	// Re-setup event listeners after DOM regeneration
	setupEventListeners();
}

function selectInitialLoadedTopic(path) {
	converter.loadMarkdown(path).then((md) => {
		currentMarkdownContent = md; // for copy button
		const html = converter.convert(md, 	);
		preview.innerHTML = html;
	});

	const rightPanelHeader = document.getElementById("right-panel-header");
	getMarkdownHeaders(path).then((headings) => {
		const html = generateHtmlRightHeader(headings);
		rightPanelHeader.innerHTML = html;
		setupRightPanelListeners(rightPanelHeader);
	});

	const dataPath = path.replace("../../assets/", "");
	const targetButton = document.querySelector(`[data-path="${dataPath}"]`);
	targetButton.classList.remove("topic-unselected");
	targetButton.classList.add("topic-selected");
	currentlySelectedTopic = targetButton;
}

document.addEventListener("DOMContentLoaded", async (event) => {
	// Get reference to the topic browser container
	browserContainer = document.querySelector(".topic-selector");

	// Fetch and load topic structure
	await browser.fetchStructure("../content-structure.json");

	// Load saved content structure state from localStorage
	const savedState = loadContentStructureState();
	let savedPagePath = null;
	let savedCategoryName = null;

	if (savedState) {
		// Merge the saved collapsed states with the freshly loaded structure
		browser.contentStructure = mergeContentStructureStates(browser.contentStructure, savedState.contentStructure);
		savedPagePath = savedState.selectedPage;
		savedCategoryName = savedState.selectedCategory;
	}

	// Generate the flat structure and pass it to the MarkdownConverter
	const flatStructure = browser.flattenStructure();
	converter = new MarkdownConverter(flatStructure, browser.contentStructure);

	await refresh();

	// Restore the previously selected page or load default
	if (savedPagePath || savedCategoryName) {
		await restoreSelectedPage(savedPagePath, savedCategoryName);
	} else {
		selectInitialLoadedTopic("../content/introduction.md");
	}

	const savedTheme = localStorage.getItem("theme");
	if (savedTheme === "dark") {
		const toggleButton = document.getElementById("lightDarkToggle");
		const copyButton = document.getElementById("copyButton");
		if (toggleButton) {
			toggleButton.innerHTML = '<img class="icon" src="../../assets/img/light.svg"><span class="icon-text">Light Mode</span>';
			copyButton.innerHTML = '<img class="icon" src="../../assets/img/copy.svg"></img><span class="icon-text">Copy</span>';
		}
	}

	document.addEventListener("click", (e) => {
		const searchContainer = document.querySelector(".header-search-bar");
		const searchPopup = document.getElementById("searchPopup");
		if (!searchContainer.contains(e.target)) {
			searchPopup.classList.remove("visible");
		}
	});

	// Sidebar toggle
	const topicToggle = document.getElementById('topicToggle');
	const sidebarOverlay = document.getElementById('sidebarOverlay');
	const topicClose = document.getElementById('topicClose');

	function openSidebar() {
		document.body.classList.add('sidebar-open');
		const firstTopic = document.querySelector('.topic-selector button, .topic-selector a');
		if (firstTopic) firstTopic.focus();
		try {
			if (topicToggle) topicToggle.setAttribute('aria-expanded', 'true');
		} catch (e) {}
	}

	function closeSidebar() {
		document.body.classList.remove('sidebar-open');
		try {
			if (topicToggle) topicToggle.setAttribute('aria-expanded', 'false');
		} catch (e) {}
	}

	if (topicToggle) {
		topicToggle.addEventListener('click', (ev) => {
			ev.stopPropagation();
			if (document.body.classList.contains('sidebar-open')) closeSidebar();
			else openSidebar();
		});
	}

	if (sidebarOverlay) {
		sidebarOverlay.addEventListener('click', () => closeSidebar());
	}

	if (topicClose) {
		topicClose.addEventListener('click', (e) => {
			e.stopPropagation();
			closeSidebar();
		});
	}

	document.addEventListener('keydown', (e) => {
		if (e.key === 'Escape') closeSidebar();
	});

	window.addEventListener('resize', () => {
		if (window.innerWidth > 768) closeSidebar();
	});
});
