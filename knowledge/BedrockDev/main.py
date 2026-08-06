"""将 BedrockDev 生成的 HTML 文档转换为 Markdown。"""

from __future__ import annotations

import argparse
import html
import re
from dataclasses import dataclass, field
from html.parser import HTMLParser
from pathlib import Path
from urllib.parse import quote


SOURCE_DIR = Path(r"D:\Documents\mc资料库\bedrockdev_github\docs\1.21.0.0\1.21.120.4")
VERSION = "1.21.120.4"
FENCED_SCHEMA_PATTERN = re.compile(
	r"^```[ \t]*\r?\n(.*?)^```[ \t]*(?=</?br\b|$)",
	re.MULTILINE | re.DOTALL | re.IGNORECASE,
)


@dataclass
class Node:
	"""保存解析后的简化 HTML 节点。"""

	tag: str
	attrs: dict[str, str] = field(default_factory=dict)
	children: list[Node | str] = field(default_factory=list)


@dataclass(frozen=True)
class PageInfo:
	"""记录索引页需要展示的单页转换结果。"""

	category: str
	title: str
	filename: str
	heading_count: int
	table_count: int


class DocumentParser(HTMLParser):
	"""把不完全规范的 BedrockDev HTML 解析成可遍历的节点树。"""

	VOID_TAGS = {"br", "hr", "img", "meta", "link", "input"}
	DOCUMENT_TAGS = {
		"a",
		"br",
		"h1",
		"h2",
		"h3",
		"h4",
		"h5",
		"h6",
		"p",
		"table",
		"td",
		"textarea",
		"th",
		"tr",
	}

	def __init__(self) -> None:
		super().__init__(convert_charrefs=True)
		self.root = Node("document")
		self.stack = [self.root]

	def handle_starttag(self, tag: str, attrs: list[tuple[str, str | None]]) -> None:
		tag = tag.lower()
		if tag not in self.DOCUMENT_TAGS:
			self.stack[-1].children.append(self.get_starttag_text())
			return
		node = Node(tag, {key: value or "" for key, value in attrs})
		self.stack[-1].children.append(node)
		if node.tag not in self.VOID_TAGS:
			self.stack.append(node)

	def handle_startendtag(self, tag: str, attrs: list[tuple[str, str | None]]) -> None:
		self.handle_starttag(tag, attrs)
		if self.stack[-1].tag == tag.lower():
			self.stack.pop()

	def handle_endtag(self, tag: str) -> None:
		tag = tag.lower()
		if tag not in self.DOCUMENT_TAGS:
			self.stack[-1].children.append(f"</{tag}>")
			return
		for index in range(len(self.stack) - 1, 0, -1):
			if self.stack[index].tag == tag:
				del self.stack[index:]
				return

	def handle_data(self, data: str) -> None:
		self.stack[-1].children.append(data)


def parse_document(source: str) -> DocumentParser:
	"""解析页面，并先保护独立行 fenced schema 中的尖括号占位符。"""

	protected = FENCED_SCHEMA_PATTERN.sub(
		lambda match: (
			'<textarea readonly="true" data-fenced-schema="true">'
			+ html.escape(match.group(1), quote=False)
			+ "</textarea>"
		),
		source,
	)
	parser = DocumentParser()
	parser.feed(protected)
	return parser


def plain_text(node: Node | str) -> str:
	"""提取节点文本，并把标签间的排版空白折叠为单个空格。"""

	if isinstance(node, str):
		return node
	if node.tag == "a" and normalized_anchor_text(node).lower() == "back to top":
		return ""
	return "".join(plain_text(child) for child in node.children)


def normalized_anchor_text(node: Node) -> str:
	"""提取锚点自身文字，避免通过 plain_text 递归判断导航链接。"""

	return re.sub(
		r"\s+",
		" ",
		"".join(child if isinstance(child, str) else normalized_anchor_text(child) for child in node.children),
	).strip()


def normalized_text(node: Node | str) -> str:
	"""返回适合标题和表格单元格的单行文本。"""

	return re.sub(r"\s+", " ", plain_text(node)).strip()


def cell_text(node: Node | str) -> str:
	"""提取单元格正文，但排除稍后单独渲染的嵌套表格。"""

	if isinstance(node, str):
		return node
	if node.tag == "table":
		return ""
	if node.tag == "a" and normalized_anchor_text(node).lower() == "back to top":
		return ""
	return "".join(cell_text(child) for child in node.children)


def table_rows(node: Node) -> list[Node]:
	"""查找当前表格的行，不跨入任何嵌套表格。"""

	rows: list[Node] = []
	for child in node.children:
		if not isinstance(child, Node) or child.tag == "table":
			continue
		if child.tag == "tr":
			rows.append(child)
		else:
			rows.extend(table_rows(child))
	return rows


def nested_tables(node: Node) -> list[tuple[str, Node]]:
	"""返回当前表格内的直接子表及其所属字段名。"""

	found: list[tuple[str, Node]] = []
	for row in table_rows(node):
		cells = [
			child
			for child in row.children
			if isinstance(child, Node) and child.tag in {"th", "td"}
		]
		if not cells:
			continue
		owner = re.sub(r"\s+", " ", cell_text(cells[0])).strip()
		for cell in cells:
			found.extend((owner, table) for table in direct_tables(cell))
	return found


def direct_tables(node: Node) -> list[Node]:
	"""查找节点内最近一层表格，不继续进入已找到的表格。"""

	found: list[Node] = []
	for child in node.children:
		if not isinstance(child, Node):
			continue
		if child.tag == "table":
			found.append(child)
		else:
			found.extend(direct_tables(child))
	return found


def table_markdown(node: Node) -> str:
	"""把 HTML 表格转换为 GitHub Flavored Markdown 表格。"""

	rows: list[list[str]] = []
	for row in table_rows(node):
		cells = [
			re.sub(r"\s+", " ", cell_text(child)).strip().replace("|", r"\|")
			for child in row.children
			if isinstance(child, Node) and child.tag in {"th", "td"}
		]
		if cells:
			rows.append(cells)

	if not rows:
		return ""

	width = max(len(row) for row in rows)
	rows = [row + [""] * (width - len(row)) for row in rows]
	lines = [
		"| " + " | ".join(rows[0]) + " |",
		"| " + " | ".join(["---"] * width) + " |",
	]
	lines.extend("| " + " | ".join(row) + " |" for row in rows[1:])
	return "\n".join(lines)


def render_table(node: Node) -> str:
	"""渲染父表，并按字段名递归追加子参数表。"""

	blocks = [table_markdown(node)]
	for owner, child in nested_tables(node):
		blocks.extend((f"> **{owner}** 子参数", render_table(child)))
	return "\n\n\n".join(block for block in blocks if block)


class MarkdownRenderer:
	"""按参考知识库的版式渲染 BedrockDev 页面。"""

	def __init__(self, version: str) -> None:
		self.version = version
		self.first_heading = True
		self.index_table_skipped = False

	def render(self, root: Node) -> str:
		blocks: list[str] = []
		for child in root.children:
			rendered = self.render_node(child).strip()
			if (
				rendered
				and rendered.lower() != "back to top"
				and rendered != "```"
				and not re.fullmatch(r"-{3,}", rendered)
			):
				blocks.append(rendered)
		result = "\n\n\n".join(blocks)
		result = re.sub(r"\n{4,}", "\n\n\n", result)
		return result.rstrip() + "\n"

	def render_node(self, node: Node | str) -> str:
		if isinstance(node, str):
			return re.sub(r"\s+", " ", html.unescape(node))

		if node.tag in {"h1", "h2", "h3", "h4"}:
			title = normalized_text(node)
			if not title:
				return ""
			prefix = "#" * int(node.tag[1])
			if self.first_heading:
				self.first_heading = False
				title = f"{title} Version: {self.version}"
			return f"{prefix} {title}"

		if node.tag == "table":
			if not self.index_table_skipped:
				self.index_table_skipped = True
				return ""
			return render_table(node)

		if node.tag == "textarea":
			content = plain_text(node).strip("\r\n ")
			if content.startswith("```") and content.endswith("```"):
				content = re.sub(r"^```(?:json)?\s*|\s*```$", "", content, flags=re.DOTALL)
			return f"```json\n{content.strip()}\n```"

		if node.tag == "code":
			return f"`{normalized_text(node)}`"

		if node.tag == "a" and normalized_text(node).lower() == "back to top":
			return ""

		if node.tag in {"br", "hr"}:
			return ""

		return "".join(self.render_node(child) for child in node.children)


def markdown_stats(markdown: str) -> tuple[int, int, int, list[str]]:
	"""统计代码块外的标题、表格和结构标签。"""

	in_code = False
	heading_count = 0
	table_count = 0
	fence_count = 0
	structural_html: list[str] = []
	structural_pattern = re.compile(r"<(?:a|br|h[1-6]|p|table|td|textarea|th|tr)\b", re.IGNORECASE)

	for line in markdown.splitlines():
		if line.startswith("```"):
			in_code = not in_code
			fence_count += 1
			continue
		if in_code:
			continue
		if re.match(r"^#{1,6} \S", line):
			heading_count += 1
		if line.startswith("| ---") and line.endswith("|"):
			table_count += 1
		structural_html.extend(match.group(0) for match in structural_pattern.finditer(line))

	return heading_count, table_count, fence_count, structural_html


def convert_file(source: Path, destination: Path, version: str) -> PageInfo:
	"""转换单个 HTML 文件、校验结构并写入 UTF-8 Markdown。"""

	parser = parse_document(source.read_text(encoding="utf-8"))
	markdown = MarkdownRenderer(version).render(parser.root)
	heading_count, table_count, fence_count, structural_html = markdown_stats(markdown)
	expected_tables = count_nodes(parser.root, "table") - 1
	expected_code_blocks = count_nodes_outside_tables(parser.root, "textarea")
	code_blocks = markdown.count("```json\n")

	problems: list[str] = []
	if not markdown.startswith("# ") or f"Version: {version}" not in markdown.splitlines()[0]:
		problems.append("首标题或版本号不正确")
	if table_count != expected_tables:
		problems.append(f"表格应为 {expected_tables}，实际为 {table_count}")
	if code_blocks != expected_code_blocks or fence_count != expected_code_blocks * 2:
		problems.append(
			f"代码块应为 {expected_code_blocks}，实际为 {code_blocks}，围栏数为 {fence_count}"
		)
	if structural_html:
		problems.append(f"仍有 HTML 结构标签: {', '.join(structural_html[:5])}")
	if "back to top" in markdown.lower():
		problems.append("仍有 Back to top 导航文本")
	if problems:
		raise ValueError(f"转换结果校验失败 {source.name}: {'; '.join(problems)}")

	destination.write_text(markdown, encoding="utf-8", newline="\n")
	first_line = markdown.splitlines()[0]
	title = first_line.removeprefix("# ").removesuffix(f" Version: {version}").title()
	return PageInfo(source.stem, title, destination.name, heading_count, table_count)


def count_nodes(node: Node, tag: str) -> int:
	"""统计解析树中的指定结构节点。"""

	return int(node.tag == tag) + sum(
		count_nodes(child, tag) for child in node.children if isinstance(child, Node)
	)


def count_nodes_outside_tables(node: Node, tag: str, inside_table: bool = False) -> int:
	"""统计表格外的指定节点，表格内示例由单元格文本直接承载。"""

	inside_table = inside_table or node.tag == "table"
	return int(node.tag == tag and not inside_table) + sum(
		count_nodes_outside_tables(child, tag, inside_table)
		for child in node.children
		if isinstance(child, Node)
	)


def write_index(destination: Path, pages: list[PageInfo], version: str) -> None:
	"""按参考知识库格式生成文档索引。"""

	lines = [
		"## 页面列表",
		"",
		"| 分类 | 标题 | 版本 | 来源 | 源页面 | 标题数 | 表格数 | Markdown |",
		"| --- | --- | --- | --- | --- | --- | --- | --- |",
	]
	for page in pages:
		source_url = f"https://bedrock.dev/docs/1.21.0.0/{version}/{quote(page.category)}"
		lines.append(
			f"| {page.category} | {page.title} | {version} | bedrock.dev | "
			f"[源页面]({source_url}) | {page.heading_count} | {page.table_count} | "
			f"[Markdown]({page.filename}) |"
		)
	destination.write_text("\n".join(lines) + "\n", encoding="utf-8", newline="\n")


def convert_directory(source: Path, destination: Path, version: str) -> list[PageInfo]:
	"""按文件名顺序批量转换目录中的全部 HTML。"""

	destination.mkdir(parents=True, exist_ok=True)
	sources = sorted(source.glob("*.html"), key=lambda path: path.name.casefold())
	if not sources:
		raise ValueError(f"源目录没有 HTML 文件: {source}")

	pages: list[PageInfo] = []
	for number, source_file in enumerate(sources, start=1):
		output_file = destination / f"{number:02d}_{source_file.stem}.md"
		page = convert_file(source_file, output_file, version)
		pages.append(page)
		print(
			f"[{number:02d}/{len(sources):02d}] {source_file.name} -> {output_file.name} "
			f"({page.heading_count} headings, {page.table_count} tables)"
		)

	write_index(destination / "index.md", pages, version)
	return pages


def main() -> None:
	"""解析命令行参数并转换单页或整个目录。"""

	parser = argparse.ArgumentParser(description=__doc__)
	parser.add_argument("source", nargs="?", type=Path, default=SOURCE_DIR)
	parser.add_argument("destination", nargs="?", type=Path, default=Path(__file__).parent)
	parser.add_argument("--version", default=VERSION)
	args = parser.parse_args()

	if args.source.is_dir():
		pages = convert_directory(args.source, args.destination, args.version)
		print(f"Converted {len(pages)} pages; index -> {args.destination / 'index.md'}")
		return

	destination = args.destination
	if destination.suffix.lower() != ".md":
		destination = destination / f"{args.source.stem}.md"
	page = convert_file(args.source, destination, args.version)
	print(
		f"Converted: {args.source.name} -> {destination} "
		f"({page.heading_count} headings, {page.table_count} tables)"
	)


if __name__ == "__main__":
	main()
