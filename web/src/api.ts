export const scopes = ["all", "api", "event", "enum", "wiki", "dev", "qumod", "netease"] as const;

export type SearchScope = (typeof scopes)[number];

export interface SearchItem {
  source: Exclude<SearchScope, "all"> | "other";
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

export interface DocumentResponse {
  path: string;
  source: SearchItem["source"];
  title: string;
  total_lines: number;
  content: string;
}

export interface MetaResponse {
  name: string;
  version: number;
  documents: number;
  scopes: SearchScope[];
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

export function fetchDocument(path: string, signal?: AbortSignal): Promise<DocumentResponse> {
  const params = new URLSearchParams({ path });
  return requestJson<DocumentResponse>(`/api/v1/document?${params}`, signal);
}
