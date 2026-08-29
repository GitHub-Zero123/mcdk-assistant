export const scopes = ["all", "api", "event", "enum", "wiki", "dev", "qumod", "netease"] as const;
export const assetScopes = ["all", "bp", "rp"] as const;

export type SearchScope = (typeof scopes)[number];
export type AssetScope = (typeof assetScopes)[number];

/** Every origin a result can come from, including the game asset index. */
export type Source = Exclude<SearchScope, "all"> | "asset" | "other";

/** The shape both result lists render, so one row component serves both. */
export interface Hit {
  source: Source;
  path: string;
  title: string;
  snippet: string;
  score: number;
  /** Documents carry a line anchor; game assets are whole files. */
  line?: number;
}

export interface SearchItem {
  source: Source;
  path: string;
  line_start: number;
  line_end: number;
  title: string;
  snippet: string;
  score: number;
}

export interface SearchResponse {
  query: string;
  scope: SearchScope;
  page: number;
  page_size: number;
  has_more: boolean;
  items: SearchItem[];
}

export interface AssetItem {
  source: "asset";
  path: string;
  title: string;
  snippet: string;
  score: number;
}

export interface AssetResponse {
  query: string;
  scope: AssetScope;
  limit: number;
  items: AssetItem[];
}

export interface DocumentResponse {
  path: string;
  source: Source;
  title: string;
  total_lines: number;
  content: string;
}

export interface MetaResponse {
  name: string;
  version: number;
  documents: number;
  assets: number;
  scopes: SearchScope[];
  asset_scopes: AssetScope[];
}

interface ApiErrorPayload {
  error?: {
    code?: string;
    message?: string;
  };
}

export class ApiError extends Error {
  constructor(
    message: string,
    readonly status: number,
  ) {
    super(message);
  }
}

async function requestJson<T>(path: string, signal?: AbortSignal): Promise<T> {
  const response = await fetch(path, {
    headers: { Accept: "application/json" },
    signal,
  });

  if (!response.ok) {
    let message = `请求失败 (${response.status})`;
    try {
      const body = (await response.json()) as ApiErrorPayload;
      if (body.error?.message) message = body.error.message;
    } catch {
      // The status code remains useful when an upstream proxy returns non-JSON.
    }
    throw new ApiError(message, response.status);
  }
  return (await response.json()) as T;
}

export function fetchMeta(signal?: AbortSignal): Promise<MetaResponse> {
  return requestJson<MetaResponse>("/api/v1/meta", signal);
}

export function searchDocuments(
  query: string,
  scope: SearchScope,
  page: number,
  pageSize: number,
  signal?: AbortSignal,
): Promise<SearchResponse> {
  const params = new URLSearchParams({
    q: query,
    scope,
    page: String(page),
    page_size: String(pageSize),
  });
  return requestJson<SearchResponse>(`/api/v1/search?${params}`, signal);
}

export function searchAssets(
  query: string,
  scope: AssetScope,
  limit: number,
  signal?: AbortSignal,
): Promise<AssetResponse> {
  const params = new URLSearchParams({ q: query, scope, limit: String(limit) });
  return requestJson<AssetResponse>(`/api/v1/assets?${params}`, signal);
}

export function fetchDocument(path: string, signal?: AbortSignal): Promise<DocumentResponse> {
  const params = new URLSearchParams({ path });
  return requestJson<DocumentResponse>(`/api/v1/document?${params}`, signal);
}

export function toHit(item: SearchItem): Hit {
  return {
    source: item.source,
    path: item.path,
    title: item.title,
    snippet: item.snippet,
    score: item.score,
    line: item.line_start,
  };
}

/**
 * Path-only asset matches come back as `[PATH MATCH] <path>`, which the row
 * already shows on its own line.
 */
export function assetToHit(item: AssetItem): Hit {
  const snippet = item.snippet.startsWith("[PATH MATCH]") ? "" : item.snippet;
  return {
    source: "asset",
    path: item.path,
    title: item.title,
    snippet,
    score: item.score,
  };
}
