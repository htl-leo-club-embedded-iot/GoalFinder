/*
 * ===============================================================================
 * (c) HTBLA Leonding 2024 - 2026
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Licensed under MIT License.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the license.
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * All trademarks used in this document are property of their respective owners.
 * ===============================================================================
 */

import { DEVICE_PUBLIC_KEY_PEM } from "../config/devicePublicKey";
import * as forge from "node-forge";

function pemToArrayBuffer(pem: string): ArrayBuffer {
    const b64 = pem.replace(/-----BEGIN PUBLIC KEY-----/g, "")
        .replace(/-----END PUBLIC KEY-----/g, "")
        .replace(/\s+/g, "");
    const binary = atob(b64);
    const len = binary.length;
    const bytes = new Uint8Array(len);
    for (let i = 0; i < len; i++) {
        bytes[i] = binary.charCodeAt(i);
    }
    return bytes.buffer;
}

async function importPublicKey(pem: string): Promise<CryptoKey> {
    const spki = pemToArrayBuffer(pem);
    return await window.crypto.subtle.importKey(
        "spki",
        spki,
        {
            name: "RSA-OAEP",
            hash: { name: "SHA-256" },
        },
        false,
        ["encrypt"],
    );
}

function arrayBufferToBase64(buffer: ArrayBuffer): string {
    let binary = "";
    const bytes = new Uint8Array(buffer);
    for (let i = 0; i < bytes.byteLength; i++) {
        binary += String.fromCharCode(bytes[i]);
    }
    return btoa(binary);
}

function isWebCryptoAvailable(): boolean {
    let available = false;
    if (typeof window !== "undefined") {
        if (window.crypto && window.crypto.subtle) {
            available = true;
        }
    }
    return available;
}

function encryptWithForge(plaintext: string, pem: string): string {
    let encrypted = "";
    const publicKey = forge.pki.publicKeyFromPem(pem);
    const encryptedBytes = publicKey.encrypt(plaintext, "RSA-OAEP", {
        md: forge.md.sha256.create(),
        mgf1: { md: forge.md.sha256.create() },
    });
    encrypted = forge.util.encode64(encryptedBytes);
    return encrypted;
}

/**
 * Encrypts plaintext with the device public key using RSA-OAEP (SHA-256).
 * Uses WebCrypto when available; falls back to node-forge for non-secure contexts.
 */
export async function encryptWithDevicePublicKey(plaintext: string): Promise<string> {
    let encrypted = "";
    if (!DEVICE_PUBLIC_KEY_PEM || DEVICE_PUBLIC_KEY_PEM.includes("REPLACE_WITH")) {
        throw new Error("Device public key not configured");
    }

    if (isWebCryptoAvailable()) {
        const key = await importPublicKey(DEVICE_PUBLIC_KEY_PEM);
        const encoded = new TextEncoder().encode(plaintext);
        const cipher = await window.crypto.subtle.encrypt({ name: "RSA-OAEP" }, key, encoded);
        encrypted = arrayBufferToBase64(cipher);
    } else {
        encrypted = encryptWithForge(plaintext, DEVICE_PUBLIC_KEY_PEM);
    }

    return encrypted;
}

/**
 * Encrypts plaintext when possible, prefixing the payload with "RSA:".
 * Falls back to plaintext to avoid blocking UI updates.
 */
export async function encryptIfAvailable(plaintext: string): Promise<string> {
    let result = plaintext;
    try {
        const b64 = await encryptWithDevicePublicKey(plaintext);
        result = `RSA:${b64}`;
    } catch (err) {
        // If encryption fails, return plaintext so UX is not blocked.
        console.warn("encryptIfAvailable: failed to encrypt, sending plaintext", err);
    }
    return result;
}
