<script lang="ts">
	import type Leaflet from 'leaflet';
	import { onMount } from 'svelte';

	let map: Leaflet.Map;
	let circle: Leaflet.Circle<any>;

	interface Props {
		devices: {
			name: string;
			lat: number;
			long: number;
		}[];
		crashSites: {
			lat: number;
			long: number;
			deviceId: string;
			time: Date | null;
		}[];
		radius: number;
		userposition: User_Position;
		class: string;
	}

	let { devices, radius, crashSites, userposition, class: className = '' }: Props = $props();

	type User_Position = {
		latitude: number;
		longitude: number;
	};

	let user_position = userposition;

	$effect(() => {
		const r = radius * 1000;
		if (circle) {
			circle.setRadius(r);
		}
	});

	onMount(async () => {
		// Dynamically import Leaflet to avoid SSR issues
		try {
			const L = await import('leaflet');
			import('leaflet/dist/leaflet.css');

			const circle_center: Leaflet.LatLngExpression = [
				user_position.latitude,
				user_position.longitude
			];
			console.log('circle', circle_center);

			map = L.map('map', {
				center: circle_center,
				zoom: 13, // Initial zoom level
				minZoom: 2.5, // Restricts zooming out too far
				maxBounds: [
					[-90, -180], // Southwest corner (entire world)
					[90, 180] // Northeast corner (entire world)
				],
				maxBoundsViscosity: 1.0 // Prevents panning beyond the bounds
			});

			const myCustomColour = '#583470';

			// ##### DEVICES #####
			const markerHtmlStyles = `
  background-color: ${myCustomColour};
  width: 2rem;
  height: 2rem;
  display: block;
  left: -1rem;
  top: -1rem;
  position: relative;
  border-radius: 3rem 3rem 0;
  transform: rotate(45deg);
  border: 1px solid #FFFFF`;

			const icon = L.divIcon({
				className: 'my-custom-pin',
				iconAnchor: [0, 24],
				popupAnchor: [0, -36],
				html: `<span style="${markerHtmlStyles}" />`
			});

			for (const element of devices ?? []) {
				if (!element.lat || !element.long) continue;

				L.marker([element.lat, element.long], { icon: icon }).addTo(map).bindPopup(element.name);
			}

			// ##### CRASHES #####
			const myCustomColourCrash = '#aebedf';
			const markerHtmlStylesCrash = `
  background-color: ${myCustomColourCrash};
  width: 2rem;
  height: 2rem;
  display: block;
  left: -1rem;
  top: -1rem;
  position: relative;
  border-radius: 2rem 2rem 0;
  transform: rotate(45deg);
  border: 1px solid #FFFFFF`;
			const iconCrash = L.divIcon({
				className: 'my-custom-pin',
				iconAnchor: [0, 24],
				popupAnchor: [0, -36],
				html: `<span style="${markerHtmlStylesCrash}" />`
			});

			for (const element of crashSites ?? []) {
				if (!element.lat || !element.long) {
					return;
				}

				L.marker([element.lat, element.long], { icon: iconCrash })
					.addTo(map)
					.bindPopup(element.deviceId + ' - ' + element.time);
			}

			// ##### Map layer #####
			L.tileLayer('https://tile.openstreetmap.org/{z}/{x}/{y}.png', {
				maxZoom: 19,
				attribution: '&copy; <a href="http://www.openstreetmap.org/copyright">OpenStreetMap</a>'
			}).addTo(map);

			L.control.scale().addTo(map);

			// Current custom position
			const myCustomColourUser = 'rgb(0,255,0)';
			const markerHtmlStylesUser = `
  background-color: ${myCustomColourUser};
  width: 2rem;
  height: 2rem;
  display: block;
  left: -1rem;
  top: -1rem;
  position: relative;
  border-radius: 3rem 3rem 0;
  transform: rotate(45deg);
  border: 1px solid #FFFFFF`;

			const iconUser = L.divIcon({
				className: 'my-custom-pin',
				iconAnchor: [0, 24],
				popupAnchor: [0, -36],
				html: `<span style="${markerHtmlStylesUser}"/>`
			});
			L.marker([user_position.latitude ?? 100, user_position.longitude ?? 100], {
				icon: iconUser
			}).addTo(map);
			circle = L.circle(circle_center, {
				color: 'green',
				fillColor: '#bfffd0',
				fillOpacity: 0.3,
				radius: radius * 1000
			}).addTo(map);
		} catch (error) {
			console.log(error);
		}
	});
</script>

<div id="map" class={className}></div>

<style>
	#map {
		height: 400px;
		width: 100%;
	}
</style>
