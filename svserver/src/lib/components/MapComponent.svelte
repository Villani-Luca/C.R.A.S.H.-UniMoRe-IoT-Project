<script lang="ts">
	import type { LayerGroup } from 'leaflet';
	import { onMount } from 'svelte';

	let map: Map | LayerGroup<any>;
	let circle;

	interface Props {
		devices: { name: string; lat: number; long: number }[];
		radius: number;
		crashSites: { lat: number; long: number }[];
	}
	let props: Props = $props();

	console.log('esempio:' + props.devices);

	type User_Position = {
		latitude: number;
		longitude: number;
	};

	onMount(async () => {
		// Dynamically import Leaflet to avoid SSR issues
		const L = await import('leaflet');
		import('leaflet/dist/leaflet.css');

		let user_position = await getUserPosition();

		const circle_center = [user_position.latitude, user_position.longitude];

		map = L.map('map', {
			center: [user_position.latitude ?? 100, user_position.longitude ?? 100], // Reggio Emilia, Italy coordinates
			zoom: 13, // Initial zoom level
			minZoom: 2.5, // Restricts zooming out too far
			maxBounds: [
				[-90, -180], // Southwest corner (entire world)
				[90, 180] // Northeast corner (entire world)
			],
			maxBoundsViscosity: 1.0 // Prevents panning beyond the bounds
		});
		if (props.devices != undefined) {
			props.devices.forEach((element) => {
				L.marker([element.lat, element.long]).addTo(map).bindPopup(element.name);
			});
		}
		if (props.crashSites != undefined) {
			props.crashSites.forEach((element) => {
				L.marker([element.lat, element.long]).addTo(map);
			});
		}

		L.tileLayer('https://tile.openstreetmap.org/{z}/{x}/{y}.png', {
			maxZoom: 19,
			attribution: '&copy; <a href="http://www.openstreetmap.org/copyright">OpenStreetMap</a>'
		}).addTo(map);

		L.control.scale().addTo(map);
		L.marker([user_position.latitude ?? 100, user_position.longitude ?? 100])
			.addTo(map)
			.bindPopup('You are here!')
			.openPopup()

		circle = L.circle(circle_center, {
			color: 'green',
			fillColor: '#bfffd0',
			fillOpacity: 0.3,
			radius: props.radius
		}).addTo(map)

	});

	async function getUserPosition() {
		return new Promise<User_Position>((resolve, reject) => {
			navigator.geolocation.getCurrentPosition((position) => {
				let user_position = {
					latitude: position.coords.latitude,
					longitude: position.coords.longitude
				};
				resolve(user_position);
			}, reject);
		});
	}
</script>

<div class="flex-grow bg-white border-2 rounded-md p-2 m-2">
	<h1 class=" font-bold">C.R.A.S.H Map Locations</h1>
	<div id="map" class="mt-1"></div>
</div>

<style>
	#map {
		height: 400px;
		width: 100%;
	}

</style>
