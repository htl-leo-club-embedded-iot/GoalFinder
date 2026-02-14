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

export class Player {
    private readonly _name: string;
    private _hits: number = 0;
    private _misses: number = 0;

    constructor(name: string) {
        this._name = name;
    }

    get name() {
        return this._name;
    }

    get hits() {
        return this._hits;
    }

    get misses() {
        return this._misses;
    }

    public addHit() {
        this._hits++;
    }

    public addMiss() {
        this._misses++;
    }
}