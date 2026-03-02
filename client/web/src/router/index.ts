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

import { createRouter, createWebHistory } from 'vue-router'
import { useWebSocketStore } from '@/stores/websocket'

let devicePasswordRequired: boolean | null = null;

const router = createRouter({
  history: createWebHistory(import.meta.env.BASE_URL),
  routes: [
    {
      path: '/auth',
      name: 'auth',
      component: () => import('@/views/AuthView.vue'),
      meta: { skipAuth: true }
    },
    {
      path: '/',
      name: 'home',
      component: () => import('@/views/HomeView.vue')
    },
    {
      path: '/games',
      name: 'games',
      component: () => import('@/views/games/GamesView.vue'),
    },
    {
      path: "/games/shot-challenge",
      name: "shot-challenge",
      component: () => import('@/views/games/ShotChallengeView.vue')
    },
    {
      path: "/games/timed-shots-challenge",
      name: "timed_shots-challenge",
      component: () => import('@/views/games/TimedShotsChallengeView.vue')
    },
    {
      path: "/games/free-play",
      name: "free-play",
      component: () => import('@/views/games/FreePlay.vue')
    },
    {
      path: '/settings',
      name: 'settings',
      redirect: '/settings/audio',
      component: () => import('@/views/settings/SettingsView.vue'),
      children: [
        {
          path: "audio",
          name: "audio",
          component: () => import('@/views/settings/AudioSettingsView.vue')
        },
        {
          path: "devices",
          name: "devices",
          component: () => import('@/views/settings/LedSettingsView.vue')
        },
        {
          path: "detection",
          name: "detection",
          component: () => import('@/views/settings/DetectionSettingsView.vue')
        },
        {
          path: "system",
          name: "system",
          component: () => import('@/views/settings/SystemSettingsView.vue')
        },
      ]
    },
    {
      path: '/about',
      name: 'about',
      component: () => import('@/views/AboutView.vue')
    },
      //404 not found
    {
      path: '/:pathMatch(.*)*',
      name: 'not-found',
      component: () => import('@/views/NotFoundView.vue')
    }
    /*{
      path: '/about',
      name: 'about',
      // route level code-splitting
      // this generates a separate chunk (About.[hash].js) for this route
      // which is lazy-loaded when the route is visited.
      //component: () => import('../views/AboutView.vue')
    }*/
  ]
})

router.beforeEach(async (to, from, next) => {
  // Skip auth check for the auth page itself
  if (to.meta.skipAuth) {
    return next();
  }

  // Already authenticated this session
  if (sessionStorage.getItem('authenticated') === 'true') {
    return next();
  }

  // Check if device requires authentication via WebSocket
  if (devicePasswordRequired === null) {
    try {
      const wsStore = useWebSocketStore();
      devicePasswordRequired = await wsStore.waitForAuthStatus();
    } catch {
      devicePasswordRequired = false;
    }
  }

  if (devicePasswordRequired) {
    return next({ name: 'auth', replace: true });
  }

  return next();
});

export default router
