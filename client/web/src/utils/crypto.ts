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

export async function encryptWithDevicePublicKey(plaintext: string): Promise<string> {
    if (!DEVICE_PUBLIC_KEY_PEM || DEVICE_PUBLIC_KEY_PEM.includes("REPLACE_WITH")) {
        throw new Error("Device public key not configured");
    }

    const key = await importPublicKey(DEVICE_PUBLIC_KEY_PEM);
    const encoded = new TextEncoder().encode(plaintext);
    const cipher = await window.crypto.subtle.encrypt({ name: "RSA-OAEP" }, key, encoded);
    return arrayBufferToBase64(cipher);
}

export async function encryptIfAvailable(plaintext: string): Promise<string> {
    try {
        const b64 = await encryptWithDevicePublicKey(plaintext);
        return `RSA:${b64}`;
    } catch (err) {
        // If encryption fails, return plaintext so UX is not blocked.
        console.warn("encryptIfAvailable: failed to encrypt, sending plaintext", err);
        return plaintext;
    }
}
