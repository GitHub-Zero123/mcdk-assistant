import sys
import tempfile
import unittest
from pathlib import Path
from unittest.mock import Mock, patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'automation'))
import update_shadersdk_docs as crawler


class ShaderCrawlerTests(unittest.TestCase):
    def test_discovery_is_scoped_decoded_and_deduplicated(self):
        html = '''<a href="API参考/入口.html#x">one</a>
        <a href="API%E5%8F%82%E8%80%83/入口.html?catalog=1">two</a>
        <a href="../1-ModAPI/接口.html">other</a>
        <a href="https://example.com/evil.html">external</a>
        <a href="%2e%2e/escape.html">escape</a>
        <a href="README.html">manual</a>'''
        self.assertEqual(crawler.collect_links(html, crawler.BASE_URL), {'API参考/入口'})

    def test_conversion_preserves_code_tables_and_links(self):
        html = '''<div class="theme-default-content"><h1>Shader</h1>
        <a href="other.html">Reference</a><img src="../image.png">
        <pre class="language-glsl"><code>void EntryPoint() {\n    surface.Albedo = vec3(1.0);\n}</code></pre>
        <table><tr><th>Field</th><th>Type</th></tr><tr><td>Albedo</td><td>vec3</td></tr></table></div>'''
        md = crawler.to_markdown(html, crawler.page_url('API参考/entry'))
        self.assertIn('    surface.Albedo = vec3(1.0);', md)
        self.assertIn('```glsl', md)
        self.assertIn('| Albedo | vec3 |', md)
        self.assertIn('/other.html)', md)
        self.assertIn('/image.png)', md)
        self.assertIn('来源：', md)

    def test_failure_and_dry_run_preserve_knowledge(self):
        with tempfile.TemporaryDirectory() as tmp:
            output = Path(tmp)
            existing = output / 'README.md'
            existing.write_text('manual', encoding='utf-8')
            sidebar = Mock(content=('<a href="%sa.html">a</a>' % crawler.BASE_URL).encode())
            with patch.object(crawler, 'OUTPUT_DIR', output), \
                 patch.object(crawler.shared._session, 'get', return_value=sidebar), \
                 patch.object(crawler.shared, 'fetch_page', side_effect=RuntimeError('offline')):
                self.assertEqual(crawler.main([]), 1)
            page = '<div class="theme-default-content"><h1>Shader</h1><p>' + 'surface ' * 20 + '</p></div>'
            with patch.object(crawler, 'OUTPUT_DIR', output), \
                 patch.object(crawler.shared._session, 'get', return_value=sidebar), \
                 patch.object(crawler.shared, 'fetch_page', return_value=page):
                self.assertEqual(crawler.main(['--dry-run']), 0)
                self.assertEqual(list(output.iterdir()), [existing])
                self.assertEqual(crawler.main([]), 0)
                self.assertTrue((output / 'a.md').exists())
            self.assertEqual(existing.read_text(encoding='utf-8'), 'manual')


if __name__ == '__main__':
    unittest.main()
